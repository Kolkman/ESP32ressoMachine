#ifndef ESPRressiot_MAIN_H
#define ESPRressiot_MAIN_H

#include <Arduino.h>
#include "heater.h"
#include <PID_v1.h>

#define CURRENTFIRMWARE "ESPressIOT-PlatFormIO"
#define DEBUG
#define MQTT_DEBUG

#define MAX_CONNECTION_RETRIES 60

// options for special modules
#define ENABLE_JSON
#define ENABLE_HTTP
#define ENABLE_TELNET
#define ENABLE_MQTT
#define ENABLE_SERIAL


// Default PID settings
//above target temp
#define S_P 50
#define S_I 0.5
#define S_D 500

// below target temp

#define S_aP 8
#define S_aI 0.1
#define S_aD 400

#define S_TSET 97.5
#define S_TBAND 1.5
#define EQUILIBRIUM_POWER 32 // this is the power at which temperature can be maintained at target. Rough estimate will do.


//
// Intervals for I/O
//
#define HEATER_INTERVAL 100
#define DISPLAY_INTERVAL 1500
#define PID_INTERVAL 500
#define COLDSTART 80
// should not be needed
#define MAX_ERROR 2





// GPIO PINS
#define HEAT_RELAY_PIN 13 // 13 + GND are close on the ESP23 DEV Board


/////////////////////////////////////////////////
// global variables

extern double gPIDint;


extern double gEqPwr;

extern double gMAXsample;

extern double gTargetTemp;
extern double gOvershoot;
extern double gInputTemp;
extern double gOutputPwr;
extern double gOldTemp;
extern double gOldPwr;
extern double gP, gI, gD;
extern double gaP, gaI, gaD;

extern unsigned long time_now;
extern unsigned long time_last;

extern int gButtonState ;
extern uint8_t mac[6];

extern boolean tuning;
extern boolean osmode;
extern boolean coldstart;
extern boolean abovetarget;
extern boolean poweroffMode;
extern boolean externalControlMode;

extern String gStatusAsJson;
extern String gDebugAsJson;

extern PID espPid;
extern HEATER heaterDriver;

#endif