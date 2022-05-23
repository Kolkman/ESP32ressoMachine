// Espresso Machine PID Tuner
// 2022 Olaf Kolkman
// This code is inspired and based on code from:
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
//
// Simple PID tuning procedure
// based on the blog entry http://brettbeauregard.com/blog/2012/01/arduino-pid-autotune-library/
//

#include "ESPressoMachine.h"
#include "heater.h"
#include <PID_v1.h>
#include "pidtuner.h"

PidTuner::PidTuner(ESPressoMachine *mach)
{
  myMachine = mach;
  TuneStep = 100.0; //Power
  TuneThres = 1;   //Temperature threshold
  AvgUpperT = 0;
  AvgLowerT = 0;
  UpperCnt = 0;
  LowerCnt = 0;
  tune_count = 0;
  tune_time = 0;
  tune_start = 0;
  tuningOn = false;
}

bool PidTuner::tuning_on()
{
  tune_time = 0;
  tune_count = 0;
  UpperCnt = 0;
  LowerCnt = 0;
  AvgUpperT = 0;
  AvgLowerT = 0;
  tuningOn = true;
  myMachine->myPID->SetMode(MANUAL);
    Serial.println("Tuner: On");
  return true;
}

bool PidTuner::tuning_off()
{
  Serial.println("Tuner: Off");
  myMachine->myPID->SetMode(AUTOMATIC);

  double dt = float(tune_time - tune_start) / tune_count;
  double dT = ((AvgUpperT / UpperCnt) - (AvgLowerT / LowerCnt));

  double Ku = 4 * (2 * TuneStep) / (dT * 3.14159);
  double Pu = dt / 1000; // units of seconds
  if (tune_count != 0 && UpperCnt != 0 && LowerCnt != 0) //agains NaN
  {
    myMachine->myConfig->nearTarget.P = 0.6 * Ku;
    myMachine->myConfig->nearTarget.I = 1.2 * Ku / Pu;
    myMachine->myConfig->nearTarget.D = 0.075 * Ku * Pu;
  }
  tuningOn = false;
  return false;
}

void PidTuner::tuning_loop()
{
  //
  // count seconds between power-on-cycles
  //
  //
  unsigned long time_now = millis();
  
  if (myMachine->inputTemp < (myMachine->myConfig->targetTemp - TuneThres))
  { // below lower threshold -> power on
    if (myMachine->outputPwr == 0)
    { // just fell below threshold
      if (tune_count == 0)
      tune_start = time_now;
      tune_time = time_now;
      tune_count++;
      AvgLowerT += myMachine->inputTemp;
      LowerCnt++;
    }
    myMachine->outputPwr = TuneStep;
    myMachine->myHeater->setHeatPowerPercentage(TuneStep);
  }
  else if (myMachine->inputTemp > (myMachine->myConfig->targetTemp + TuneThres))
  { // above upper threshold -> power off
    if (myMachine->outputPwr == TuneStep)
    { // just crossed upper threshold
      AvgUpperT += myMachine->inputTemp;
      UpperCnt++;
    }
    myMachine->outputPwr = 0;
    myMachine->myHeater->setHeatPowerPercentage(0);
  }
}

unsigned long PidTuner::timeElapsed()
{
  return (abs(tune_time - tune_start));
}

float PidTuner::averagePeriod()
{
  if (!tune_count) return -1;
  return float(abs(tune_time - tune_start) / tune_count);
}

float PidTuner::upperAverage()
{
   if (!UpperCnt) return -1;
  return (AvgUpperT / UpperCnt);
}

float PidTuner::lowerAverage()
{
     if (!LowerCnt) return -1;
  return (AvgLowerT / LowerCnt);
}

void PidTuner::setTuneCount(int i)
{
  tune_count = i;
}
int PidTuner::getTuneCount()
{
  return (tune_count);
}

void PidTuner::setTuneStep(double i)
{
  TuneStep = i;
}
void PidTuner::setTuneThres(double i)
{
  TuneThres = i;
}
double PidTuner::getTuneStep()
{
  return (TuneStep);
}
double PidTuner::getTuneThres()
{
  return (TuneThres);
}

float PidTuner::averagePeakToPeak()
{
  if (!UpperCnt || ! LowerCnt) return MAXFLOAT;
  return ((AvgUpperT / UpperCnt) - (AvgLowerT / LowerCnt));
}