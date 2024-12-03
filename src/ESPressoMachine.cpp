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

#undef DEBUG

ESPressoMachine::ESPressoMachine()
{
    outputTemp = 0;
    inputTemp = 0;
    internalTemp=0;
    myHeater = new Heater();
    myConfig = new EspressoConfig;
    mySensor = new TempSensor;
    myPID = new PID(&inputTemp, &outputPwr, myConfig->ptargetTemp, 1.0, 0.0, 0.0, P_ON_E, DIRECT);
    myTuner = new PidTuner(this);
    myInterface = new ESPressoInterface(this);
    time_last = millis();
    time_now = millis();
    pwrSafeTimer = millis();
    osmode = false;
    outputPwr = 0.0;
    oldTemp = 0.0;
    oldPwr = 0;
    externalControlMode = false;
    buttonState = false;
    powerOffMode = false;
    externalControlMode = false;
    coldstart = true;
#ifdef ENABLE_NTPCLOCK
    clock = new ntpClock();
#endif
}

void ESPressoMachine::startMachine()
{
    try
    {
        mySensor->setupSensor();
    }
    catch (char *errstr)
    {
        if (strcmp(errstr, "ThermoCoupleFail"))
        {
            Serial.println("Couldnot Setup sensor (Thermo Couple Fail");
            oldTemp = 19; // Some random but descent value
        }
        else
            throw;
    }
    myPID->SetTunings(myConfig->nearTarget.P, myConfig->nearTarget.I, myConfig->nearTarget.D);
    myPID->SetSampleTime(myConfig->pidInt);
    myPID->SetOutputLimits(0, 1000);
    myPID->SetMode(AUTOMATIC);
    myInterface->setup();

#ifdef ENABLE_NTPCLOCK
    clock->setup();
#endif // ENABLE_NTPCLOCK

    outputPwr = myConfig->eqPwr;
    time_now = millis();
    time_last = time_now;
    Serial.println("Machine Started");
};

void ESPressoMachine::manageTemp()
{

    inputTemp = mySensor->getTemp(oldTemp);
    internalTemp = mySensor->getITemp(0.0);


    if (!(abs(myConfig->targetTemp - inputTemp) >= myConfig->temperatureBand) &&
        ((oldTemp - inputTemp) > (myConfig->maxCool * myConfig->pidInt / 1000)))
    {
        inputTemp = oldTemp - myConfig->maxCool * myConfig->pidInt / 1000;
        oldTemp = inputTemp;
#ifdef DEBUG
        Serial.println("Smoothing Downward");
#endif
    }
    else
    {
        oldTemp = inputTemp;
    }
}

// This method reports the MAchine's innternal status to the world. It knows about attributes
// from all parent classess too...

void ESPressoMachine::setMachineStatus()
{
    machineStatus = "";
    JsonDocument statusObject;
    statusObject["time"] = time_now;
    statusObject["measuredTemperature"] = inputTemp;
    statusObject["intTemperature"] = internalTemp;
    statusObject["targetTemperature"] = myConfig->targetTemp;
    statusObject["heaterPower"] = outputPwr;
    statusObject["externalControlMode"] = externalControlMode;
    statusObject["externalButtonState"] = buttonState;
    statusObject["powerOffMode"] = powerOffMode;
    statusObject["tuning"] = myTuner->tuningOn;
    statusObject["heap"] = ESP.getFreeHeap();
    statusObject["heapMaxAl"] = ESP.getMaxAllocHeap();
    statusObject["FloatingAvg"] = getAverage();

    serializeJson(statusObject, machineStatus);
}

bool ESPressoMachine::heatLoop()
{
    time_now = millis();
    bool returnval = false; // returns true when PID cycle was executed.
#ifdef ENABLE_SWITCH_DETECTION
    loopSwitch();
#endif

    mySensor->updateTempSensor(myConfig->sensorSampleInterval);

    if ((max(time_now, time_last) - min(time_now, time_last)) >= myConfig->pidInt or time_last > time_now)
    {
        manageTemp();

        // Powersafe mode Check if the temperature has been stable: then turn off, reset timer if average out of bound.
        if (!powerOffMode)
        {
            if (abs(myConfig->targetTemp - getAverage()) > TEMPERATURE_VAR)
            {
                pwrSafeTimer = time_now;
                //LOGDEBUG0("Resetting PowerSafeTimer");
            }
            if ((time_now - pwrSafeTimer) > 1000 * 60 * POWERSAFE_TIMEOUT)
            {
                powerOffMode = true;
            }
        }
        updatePIDSettings();
        if (powerOffMode)
        {
            outputPwr = 0;
            myHeater->setHeatPowerPercentage(0);
        }
        else if (externalControlMode)
        {
            outputPwr = 1000 * buttonState; // <======= TODO objectify
            myHeater->setHeatPowerPercentage(outputPwr);
        }
        else if (myTuner->tuningOn)
        {
            myTuner->tuning_loop();
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

        stats entry = {time_now, inputTemp, outputPwr};

        addStatistic(entry);
        returnval = true;
    }
    myHeater->updateHeater();
#ifdef ENABLE_NTPCLOCK
    clock->loop();
#endif // ENABLE_NTPCLOCK

    return (returnval);
}

void ESPressoMachine::updatePIDSettings()
{

#ifdef DEBUG
    Serial.print("UpdatePIDSettings - input,target: " + String(inputTemp) + "," + String(myConfig->targetTemp) + ", " + String(COLDSTART_TEMP_OFFSET));
    Serial.print(" coldstart,osmode =");
    if (coldstart)
        Serial.print("true");
    else
        Serial.print("false");
    Serial.print(",");
    if (osmode)
        Serial.print("true");
    else
        Serial.print("false");
    Serial.println();
#endif

    if (!coldstart && (inputTemp < (myConfig->targetTemp - COLDSTART_TEMP_OFFSET)))
    {
        Serial.println("Coldstart regime maximum power:" + String(COLDSTART_MAX_POWER));
        // Below coldstart treshold
        // Coldstart treshold is defined in ESPressoMachineDefaults
        myPID->SetTunings(COLDSTART_P, 0, 0, P_ON_E);
        myPID->SetOutputLimits(0, COLDSTART_MAX_POWER); // Go full power
        coldstart = true;
    }
    else if (coldstart && (inputTemp > (myConfig->targetTemp - COLDSTART_TEMP_OFFSET)))
    {
        Serial.println("Exiting coldstart regime");
        coldstart = false;
        osmode = (abs(myConfig->targetTemp - inputTemp) >= myConfig->temperatureBand);
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
#ifdef DEBUG
    Serial.println("P,I,D= " + String(myPID->GetKp()) + "," + String(myPID->GetKi()) + "," + String(myPID->GetKd()));
#endif
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

StatsStore::StatsStore()
{
    skip = 0;
    for (int i = 0; i < AVERAGE_TEMP_ENTRIES; i++)
    {
        AverageTempBuffer[i] = 0;
    }
    AverageTempBufferEntry = 0;

    storage[0] = '[';
    for (int i = 1; i < (STATS_SIZE - 1); i++)
    {
        storage[i] = ' ';
    }
    storage[STATS_SIZE - 1] = ']';
    storage[STATS_SIZE] = '\0';
}

char *StatsStore::getStatistics()
{
    return storage;
}

double StatsStore::getAverage()
{
    double sum;
    for (int i = 0; i < AVERAGE_TEMP_ENTRIES; i++)
    {
        sum += AverageTempBuffer[i];
    }

    return (sum / AVERAGE_TEMP_ENTRIES);
}

void StatsStore::addStatistic(stats s)
{

    AverageTempBuffer[AverageTempBufferEntry] = s.temp;
    AverageTempBufferEntry++;
    if (AverageTempBufferEntry == AVERAGE_TEMP_ENTRIES)
    {
        AverageTempBufferEntry = 0;
    }

    char statline[STAT_LINELENGTH + 1];

    // This Stringformat is 45 char's long. Modify it and things break.
    sprintf(statline, "{\"t\":%10.lu,\"T\":%.3e,\"p\":%.3e}", s.time, s.temp, s.power);

    // Move all statlines one entry to the right. start with moving the last but one entry
    // also take the succeeding comma
    if (skip < 2) // skip a few times, during startup the first string is not written
    {
        skip++;
        char *r = strncpy((storage + 1 + (STAT_ENTRIES - 1) * (STAT_LINELENGTH + 1)), statline, STAT_LINELENGTH);
    }
    else
    {
        char *s;
        char *d;
        for (int i = 0; i < (STAT_ENTRIES - 2); i++)
        {
            s = storage + 1 + (i + 1) * (STAT_LINELENGTH + 1);
            d = storage + 1 + i * (STAT_LINELENGTH + 1);
            strncpy(d, s, STAT_LINELENGTH + 1);
        }
        // Now copy the last but item to the second but last one and inject a comma

        storage[STATS_SIZE - STAT_LINELENGTH - 2] = ',';

        s = storage + (STATS_SIZE - STAT_LINELENGTH - 1);
        d = s - STAT_LINELENGTH - 1;
        strncpy(d, s, STAT_LINELENGTH);

        // Now copy the new data in place
        d = (storage + 1 + (STAT_ENTRIES - 1) * (STAT_LINELENGTH + 1));
        char *r = strncpy(d, statline, STAT_LINELENGTH);
    }
}