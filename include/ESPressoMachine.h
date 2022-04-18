#ifndef ESPressoMachine_MAIN_H
#define ESPressoMachine_MAIN_H

#include <Arduino.h>
#include "heater.h"
#include "config.h"
#include "pidtuner.h"
#include <PID_v1.h>
#include "ESPressoMachineDefaults.h"
#include "sensor_max31855.h"
#include "interface.h"

#define CURRENTFIRMWARE "ESPressoMachine-PlatFormIO"
#define DEBUG
#define MQTT_DEBUG

#define MAX_CONNECTION_RETRIES 60

// should not be needed
#define MAX_ERROR 2

/////////////////////////////////////////////////
// global variables

// extern double gPIDint;

// extern double gEqPwr;

// extern double gMAXsample;

// extern unsigned long time_now;
// extern unsigned long time_last;

extern int gButtonState;
extern uint8_t mac[6];

extern boolean externalControlMode;

class ESPressoInterface; // forward declaratin

class ESPressoMachine
{
public:
  ESPressoMachine();
  void manageTemp();
  String statusAsJson();
  void startMachine();
  bool heatLoop(); // returnes true if the loop executed something

  void reConfig(); // Apply all config in the Config object to the machine

  PID *myPID;
  Heater *myHeater;
  EspressoConfig *myConfig; // contains all configurable variables
  TempSensor *mySensor;
  PidTuner *myTuner;
  ESPressoInterface *myInterface;

  // variables to maintain internal state <== TODO move to private
  unsigned long time_now;
  unsigned long time_last;
  double outputPwr;
  double oldTemp;
  double oldPwr;
  double inputTemp;
  double outputTemp;
  bool tuning;
  bool poweroffMode;
  bool externalControlMode;
  int buttonState;

private:
  void updatePIDSettings();
  bool coldstart;
  bool osmode;
};

#endif