// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//
#include <config.h>
#include <ESPressoMachine.h>
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>


EspressoConfig::EspressoConfig()
{
  resetConfig();
  ptargetTemp = &targetTemp;
}

EspressoConfig::~EspressoConfig()
{
  Serial.println("Deleteing an instance of EspressoConfig");
  delete ptargetTemp;
}

bool EspressoConfig::prepareFS()
{
  if (!LittleFS.begin(false /* false: Do not format if mount failed */))
  {
    Serial.println("Failed to mount LittleFS");
    if (!LittleFS.begin(true /* true: format */))
    {
      Serial.println("Failed to format LittleFS");
      return false;
    }
    else
    {
      Serial.println("LittleFS formatted successfully");
      return true;
    }
  }
  else
  { // Initial mount success
    return true;
  }
}



bool EspressoConfig::loadConfig()
{
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile)
  {
    Serial.println("Failed to open config file");
    return false;
  }

  DynamicJsonDocument jsonDocument(CONFIG_BUF_SIZE);
  ReadLoggingStream loggingStream(configFile, Serial);

  DeserializationError parsingError = deserializeJson(jsonDocument, loggingStream);
  if (parsingError)
  {
    Serial.println("Failed to deserialize json config file");
    Serial.println(parsingError.c_str());
    return false;
  }

  targetTemp = jsonDocument["tset"];
  temperatureBand = jsonDocument["tband"];
  nearTarget.P = jsonDocument["P"], nearTarget.I = jsonDocument["I"], nearTarget.D = jsonDocument["D"];
  awayTarget.P = jsonDocument["aP"], awayTarget.I = jsonDocument["aI"], awayTarget.D = jsonDocument["aD"];
  eqPwr = jsonDocument["Ep"], heaterInterval = jsonDocument["hi"], pidInt = jsonDocument["pidi"],
  sensorSampleInterval = jsonDocument["ssi"];
  maxCool = jsonDocument["maxcool"];
  return true;
}

bool EspressoConfig::saveConfig()
{
  DynamicJsonDocument jsonDocument(CONFIG_BUF_SIZE);
  jsonDocument["tset"] = targetTemp;
  jsonDocument["tband"] = temperatureBand;
  jsonDocument["P"] = nearTarget.P, jsonDocument["I"] = nearTarget.I, jsonDocument["D"] = nearTarget.D;
  jsonDocument["aP"] = awayTarget.P, jsonDocument["aI"] = awayTarget.I, jsonDocument["aD"] = awayTarget.D;
  jsonDocument["Ep"] = eqPwr, jsonDocument["hi"] = heaterInterval, jsonDocument["pidi"] = pidInt,
  jsonDocument["ssi"] = sensorSampleInterval;
  jsonDocument["maxcool"] = maxCool;
  File configFile = LittleFS.open("/config.json", "w", true);
  if (!configFile)
  {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  WriteLoggingStream loggedFile(configFile, Serial);

  size_t writtenBytes = serializeJson(jsonDocument, loggedFile);

  if (writtenBytes == 0)
  {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  Serial.println("Bytes written: " + String(writtenBytes));

  configFile.close();
  return true;
}

void EspressoConfig::resetConfig()
{
  nearTarget = (PIDval){.P = S_P, .I = S_I, .D = S_D};
  awayTarget = (PIDval){.P = S_aP, .I = S_aI, .D = S_aD};
  targetTemp = S_TSET;
  temperatureBand = S_TBAND;
  eqPwr = EQUILIBRIUM_POWER;
  mxPwr = MAX_POWER;
  pidInt = PID_INTERVAL;
  heaterInterval = HEATER_INTERVAL;
  maxCool = MAX_COOL;
  sensorSampleInterval = MAX31855_SMP_TIME; ///<=== TODO
}
