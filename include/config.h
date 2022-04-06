#ifndef ESPressiotCONFIG_H
#define ESPressiotCONFIG_H

#include <StreamUtils.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define FORMAT_SPIFFS_IF_FAILED true
#define BUF_SIZE 2048

bool prepareFS();
bool loadConfig(); 
bool saveConfig();
void resetConfig();




#endif