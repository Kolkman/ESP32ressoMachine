#ifndef ESPressoMachineCONFIG_H
#define ESPressoMachineCONFIG_H

#include <StreamUtils.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>

#define FORMAT_SPIFFS_IF_FAILED true
#define CONFIG_BUF_SIZE 2048

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
    bool loadConfig(); // load current config to disk (limited set of vallues)
    bool saveConfig(); // save current config to disk (limited set of vallues)
    void resetConfig(); //resetConfig to values that were programmed by default
//    String statusAsJson();
    PIDval nearTarget;
    PIDval awayTarget;
    double targetTemp;
    double *ptargetTemp;
    double temperatureBand;
    double eqPwr;
    double mxPwr;
    double overShoot;
    double sensorSampleInterval;
    unsigned int heaterInterval;
    int pidInt;

private:
};

#endif