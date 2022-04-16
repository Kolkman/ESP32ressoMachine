// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Helper Functions
//

#include <Arduino.h>
//#include <ArduinoJson.h>
//#include <StreamUtils.h>
#include <helpers.h>


String macToID(const uint8_t* mac) {
  String result;
  for (int i = 3; i < 6; ++i) {
    result += String(mac[i], 16);
  }
  result.toUpperCase();
  return result;
}

String macToString(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5) result += ":";
  }
  result.toUpperCase();
  return result;
}


