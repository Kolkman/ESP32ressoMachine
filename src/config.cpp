

#include <config.h>
#include <ESPressiot.h>

bool prepareFS() {
  
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("Failed to mount file system");
    return false;
  }
  return true;
}

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  DynamicJsonDocument jsonDocument(BUF_SIZE);
  ReadLoggingStream loggingStream(configFile, Serial);

  DeserializationError parsingError = deserializeJson(jsonDocument, loggingStream);
  if (parsingError) {
    Serial.println("Failed to deserialize json config file");
    Serial.println(parsingError.c_str());
    return false;
  }

  gTargetTemp = jsonDocument["tset"];
  gOvershoot = jsonDocument["tband"];
  gP = jsonDocument["P"], gI = jsonDocument["I"], gD = jsonDocument["D"];
  gaP = jsonDocument["aP"], gaI = jsonDocument["aI"], gaD = jsonDocument["aD"];
  gEqPwr = jsonDocument["Ep"];

  return true;
}

bool saveConfig() {
  DynamicJsonDocument jsonDocument(BUF_SIZE);
  jsonDocument["tset"] = gTargetTemp;  jsonDocument["tband"] = gOvershoot;
  jsonDocument["P"] = gP, jsonDocument["I"] = gI, jsonDocument["D"] = gD;
  jsonDocument["aP"] = gaP, jsonDocument["aI"] = gaI, jsonDocument["aD"] = gaD;
  jsonDocument["Ep"] = gEqPwr;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  WriteLoggingStream loggedFile(configFile, Serial);

  size_t writtenBytes = serializeJson(jsonDocument, loggedFile);

  if (writtenBytes == 0) {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  Serial.println("Bytes written: " + String(writtenBytes));

  configFile.close();
  return true;
}

void resetConfig() {
  gP = S_P; gI = S_I; gD = S_D;
  gaP = S_aP; gaI = S_aI; gaD = S_aD;
  gTargetTemp = S_TSET;
  gOvershoot = S_TBAND;
  gEqPwr = EQUILIBRIUM_POWER;
}