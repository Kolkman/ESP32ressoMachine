#pragma once
#ifndef ESPressoMachineCONFIG_H
#define ESPressoMachineCONFIG_H

#include <StreamUtils.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "ESPressoMachineDefaults.h"
#include "debug.h"
#include "wifiManager.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define CONFIG_BUF_SIZE 2048

#ifndef S_P
#define S_P 50
#endif
#ifndef S_I
#define S_I 0.5
#endif
#ifndef S_D
#define S_D 500
#endif

// below target temp

#ifndef S_aP
#define S_aP 8
#endif
#ifndef S_aI
#define S_aI 0.1
#endif
#ifndef S_aD
#define S_aD 400
#endif

#ifndef S_TSET
#define S_TSET 98.5
#endif
#ifndef S_TBAND
#define S_TBAND 1.5
#endif

#ifndef EQUILIBRIUM_POWER
#define EQUILIBRIUM_POWER 32.5
#endif

#ifndef PID_INTERVAL
#define PID_INTERVAL 500
#endif

#ifndef HEATER_INTERVAL
#define HEATER_INTERVAL 100
#endif

#ifndef MAX_COOL
#define MAX_COOL 0.025 // 1.5 deg/min
#endif

#ifdef ENABLE_MQTT
#ifndef MQTT_TOPIC
#define MQTT_TOPIC "EspressoMach"
#endif
#ifndef MQTT_STATUS_TOPIC
#define MQTT_STATUS_TOPIC "/status"
#endif
#ifndef MQTT_CONFIG_TOPIC
#define MQTT_CONFIG_TOPIC "/config"
#endif
// Max length of the strings above.
#ifndef MQTT_TOPIC_EXT_LENGTH
#define MQTT_TOPIC_EXT_LENGTH 7
#endif

#endif // ENABLE_MQTT

// Struct to store pid values
struct PIDval
{
       double P, I, D;
};

// Class to handle all things that have to do with configuration
class EspressoConfig
{
public:
       EspressoConfig();
       ~EspressoConfig();
       bool prepareFS();
       bool loadConfig();  // load current config to disk (limited set of vallues)
       bool saveConfig();  // save current config to disk (limited set of vallues)
       void resetConfig(); // resetConfig to values that were programmed by default
       String passForSSID(String);
       //    String statusAsJson();
       PIDval nearTarget;
       PIDval awayTarget;
       double targetTemp;
       double *ptargetTemp;
       double temperatureBand;
       double eqPwr;
       double mxPwr;
       double maxCool;
       unsigned int sensorSampleInterval;
       unsigned int heaterInterval;
       int pidInt;

#ifdef ENABLE_MQTT
       char mqttHost[256];
       char mqttTopic[256];
       unsigned int mqttPort;
       char mqttUser[65];
       char mqttPass[65];
       char webUser[65];
       char webPass[65];
#endif                                  // ENABLE_MQTT
       WiFi_AP_IPConfig WM_AP_IPconfig; // WifiManager Configuration
       WiFi_STA_IPConfig WM_STA_IPconfig;
       WM_Config WM_config;

private:
};

#endif
