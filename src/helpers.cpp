// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Helper Functions
//

#include <Arduino.h>
#include <ESPressiot.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>
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

String statusAsJson() {
  StaticJsonDocument<1024> statusObject;
  String outputString;
  statusObject["time"] = time_now;
  statusObject["measuredTemperature"] = gInputTemp;
  statusObject["targetTemperature"] = gTargetTemp;
  statusObject["heaterPower"] = gOutputPwr;
  statusObject["externalControlMode"] = externalControlMode;
  statusObject["externalButtonState"] = gButtonState;
  statusObject["PowerOffMode"] = poweroffMode;

  serializeJson(statusObject, outputString);
  return outputString;
}

