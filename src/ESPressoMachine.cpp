#include "ESPressoMachine.h"
#include "ESPressoMachineDefaults.h"
#include <PID_v1.h>
#include "sensor_max31855.h"
#include "heater.h"
#include "pidtuner.h"
#include "interface.h"

// An Espresso machine consists out of a heater, a thermometer, and a PID controller
// The machine also has an interface and a bunch of configuration state to maintain.
// The object model reflects that.
//

ESPressoMachine::ESPressoMachine()
{
    outputTemp = 0;
    inputTemp = 0;
    myHeater = new Heater();
    myConfig = new EspressoConfig;
    mySensor = new TempSensor;
    myPID = new PID(&inputTemp, &outputPwr, myConfig->ptargetTemp, 1.0, 0.0, 0.0, P_ON_E, DIRECT);
    myTuner = new PidTuner(this);
    myInterface = new ESPressoInterface(this);
    time_last = millis();
    time_now = millis();
    osmode = false;
    outputPwr = 0.0;
    oldTemp = 0.0;
    oldPwr = 0;
    tuning = false;
    externalControlMode = false;
}

void ESPressoMachine::startMachine()
{
    mySensor->setupSensor();
    myPID->SetTunings(myConfig->nearTarget.P, myConfig->nearTarget.I, myConfig->nearTarget.D);
    myPID->SetSampleTime(myConfig->pidInt);
    myPID->SetOutputLimits(0, 1000);
    myPID->SetMode(AUTOMATIC);
    myInterface->setup();
    outputPwr = myConfig->eqPwr;
    time_now = millis();
    time_last = time_now;
    Serial.println("Machine Started");
};

void ESPressoMachine::manageTemp()
{
    mySensor->updateTempSensor(myConfig->sensorSampleInterval);
    inputTemp = mySensor->getTemp(oldTemp);
    coldstart = (inputTemp < (myConfig->targetTemp - COLDSTART_TEMP_OFFSET));
    osmode = ( abs(myConfig->targetTemp - inputTemp) >= myConfig->temperatureBand);

/*
#ifdef DEBUG
    Serial.print("inputTemp: " + String(inputTemp) + " coldstart,osmode=");
    if (coldstart)
        Serial.print("true");
    else
        Serial.print("false");
    Serial.print(",");
    if (osmode)
        Serial.print("true");
    else
        Serial.print("false");
    Serial.println(" Target: "+String(myConfig->targetTemp)+" Oversh: "+String(myConfig->temperatureBand));
#endif
*/
    // If we are not in overshoot-mode we only accept a certain amount of downward temperature gradient.
    // (Natural cooling of the boiler)
    // If the gradient is faster we top it off.
    if (!osmode &&
        ((oldTemp - inputTemp) > (0.040 * myConfig->pidInt / 1000)))
    {
        inputTemp = oldTemp - 0.040 * myConfig->pidInt / 1000;
        oldTemp = inputTemp;
    }
    else
    {
        oldTemp = inputTemp;
    }
}

// This method reports the MAchine's innternal status to the world. It knows about attributes
// from all parent classess too...

String ESPressoMachine::statusAsJson()
{
    StaticJsonDocument<1024> statusObject;
    String outputString;
    statusObject["time"] = time_now;
    statusObject["measuredTemperature"] = inputTemp;
    statusObject["targetTemperature"] = myConfig->targetTemp;
    statusObject["heaterPower"] = outputPwr;
    statusObject["externalControlMode"] = externalControlMode;
    statusObject["externalButtonState"] = gButtonState;
    statusObject["PowerOffMode"] = poweroffMode;

    serializeJson(statusObject, outputString);
    return outputString;
}

bool ESPressoMachine::heatLoop()
{
    time_now = millis();

#ifdef ENABLE_SWITCH_DETECTION
    loopSwitch();
#endif

    if ((max(time_now, time_last) - min(time_now, time_last)) >= myConfig->pidInt or time_last > time_now)
    {
        updatePIDSettings();
        if (poweroffMode == true)
        {
            outputPwr = 0;
            myHeater->setHeatPowerPercentage(0);
        }
        else if (externalControlMode)
        {
            outputPwr = 1000 * gButtonState; // <======= TODO objectify
            myHeater->setHeatPowerPercentage(outputPwr);
        }
        else if (tuning == true)
        {
            // tuning_loop();  //<<< ==== TODO
        }
        else if (myPID->Compute() == true)
        {
            if (inputTemp > FAILSAFE_TEMP)
            {
                Serial.println("FAILSAFE" + String(FAILSAFE_TEMP));
                outputPwr = 0; // FAILSAFE
            }
            myHeater->setHeatPowerPercentage(outputPwr);
        }
        time_last = time_now;
        return(true);
    }else{
        return(false);
    }
}

void ESPressoMachine::updatePIDSettings()
{
    manageTemp();

    if (!coldstart && (inputTemp < (myConfig->targetTemp - COLDSTART_TEMP_OFFSET)))
    {
        // Below coldstart treshold
        // Coldstart treshold is defined in ESPressoMachineDefaults
        myPID->SetTunings(COLDSTART_P, 0, 0, P_ON_E);
        myPID->SetOutputLimits(0, COLDSTART_MAX_POWER); // Go full power
        coldstart = true;
    }
    else if (coldstart && (inputTemp > (myConfig->targetTemp - COLDSTART_TEMP_OFFSET)))
    {
        coldstart = false;
        osmode = (abs(myConfig->targetTemp = -inputTemp) >= myConfig->temperatureBand);
    }

    if (!coldstart && !osmode && abs(myConfig->targetTemp - inputTemp) >= myConfig->temperatureBand)
    {
        // between coldstart and still more than overshoot away from the target
        // osmode  is therefore true, and we set values accordingly.
        myPID->SetTunings(myConfig->awayTarget.P, myConfig->awayTarget.I, myConfig->awayTarget.D, P_ON_E);
        if (inputTemp < myConfig->targetTemp)
            myPID->SetOutputLimits(0, myConfig->mxPwr); // We want to be able to get to the target fast
        if (inputTemp > myConfig->targetTemp)
            myPID->SetOutputLimits(0, myConfig->eqPwr); // We don't want noise to
        osmode = true;
    }
    else if (osmode && abs(myConfig->targetTemp - inputTemp) < myConfig->temperatureBand)
    {
        // Within the defined/configured offset from the target temperature

        // We are going to reinitialize the PID at the predetermined equilibrium temperature.  This
        // has a slight positive impact on convergence.
        myPID->SetMode(0);
        outputPwr = myConfig->eqPwr;
        myPID->SetMode(1);

        myPID->SetTunings(myConfig->nearTarget.P, myConfig->nearTarget.I, myConfig->nearTarget.D, P_ON_E);
        myPID->SetOutputLimits(5, 90); // Choose to not make these configurable.
                                       // We want the max power to be such that noise will not
                                       // disturb the system too much.
        osmode = false;
    }
}

void ESPressoMachine::reConfig()
{
    myPID->SetSampleTime(myConfig->pidInt);
    myHeater->setHeaterInterval(myConfig->heaterInterval);
    if (abs(myConfig->targetTemp - inputTemp) >= myConfig->temperatureBand)
    {
        myPID->SetTunings(myConfig->awayTarget.P, myConfig->awayTarget.I, myConfig->awayTarget.D, P_ON_E);
    }
    else
    {
        // force reinitialize PID at equibrilibrium-power (manually determined)

        myPID->SetTunings(myConfig->nearTarget.P, myConfig->nearTarget.I, myConfig->nearTarget.D, P_ON_M);
    }
}