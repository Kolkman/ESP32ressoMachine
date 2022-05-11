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



struct stats
{
  unsigned long time;
  double temp;
  double power;
};





// This is the length of the statline
// in StatsStore::addStatistics "{\"t\":%.10u,\"T\":%.3e,\"p\":%.3e}"
// its exact size is critical - without trailing 0.
#define STAT_LINELENGTH 44
#define STAT_ENTRIES 400 // must be larger than 1
// Amount of statlines to keep around. 
#define STATS_SIZE STAT_ENTRIES*(STAT_LINELENGTH+1) + 1  //   

class StatsStore {
  // class to store stats - all static assigned
  public:
    StatsStore();
    void addStatistic(stats);
    char * getStatistics();

  private:
   char storage[STATS_SIZE];
   int skip;
};




class ESPressoInterface; // forward declaratin


// The EspressoMachine Class is the storage of all components and
// data, hence it is alsoo a StatsStore
class ESPressoMachine: public StatsStore
{
public:
  ESPressoMachine();
  void manageTemp();
  String statusAsJson();
  void startMachine();
  bool heatLoop(); // returnes true if the loop executed something

  void reConfig(); // Apply all config in the Config object to the machine

  // An esspresso machine has a PID, a boiler/heater, a config, a sensor
  // a tuner, and an interface.
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
  bool powerOffMode;
  bool externalControlMode;
  int buttonState;

private:
  void updatePIDSettings();
  bool coldstart;
  bool osmode;
};

#endif