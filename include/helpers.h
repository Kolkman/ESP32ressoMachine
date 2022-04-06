#ifndef ESPressiot_HELPERS_H
#define ESPressiot_HELPERS_H
#include <Arduino.h>

String macToID(const uint8_t* mac);
String macToString(const uint8_t* mac);
String statusAsJson();

#endif