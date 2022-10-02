// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//
#include <config.h>
#include "wifiManager.h"
#include <ESPressoMachine.h>
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

EspressoConfig::EspressoConfig()
{
  resetConfig();

  strcpy(webUser, WEB_USER);
  strcpy(webPass, WEB_PASS);

#ifdef ENABLE_MQTT
  // These values we want to initialize but not reset:
  strcpy(mqttHost, MQTT_HOST);
  strcpy(mqttPass, MQTT_PASS);
  strcpy(mqttUser, MQTT_USER);
  strcpy(mqttTopic, MQTT_TOPIC);
  mqttPort = 1883;
#endif // ENABLE_MQTT

  ptargetTemp = &targetTemp;

  /* for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++)
  {
    *WM_config.WiFi_Creds[i].wifi_ssid = 0;
    *WM_config.WiFi_Creds[i].wifi_pw = 0;
  }
  *WM_config.TZ_Name = 0;
  *WM_config.TZ = 0;
*/

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

  StaticJsonDocument<CONFIG_BUF_SIZE> jsonDocument;
  ReadLoggingStream loggingStream(configFile, Serial);

  DeserializationError parsingError = deserializeJson(jsonDocument, loggingStream);
  if (parsingError)
  {
    Serial.println("Failed to deserialize json config file");
    Serial.println(parsingError.c_str());
    return false;
  }

  if (jsonDocument["tset"])
  {
    targetTemp = jsonDocument["tset"];
  }
  else
  {
    targetTemp = S_TSET;
  }

  temperatureBand = jsonDocument["tband"];
  nearTarget.P = jsonDocument["P"], nearTarget.I = jsonDocument["I"], nearTarget.D = jsonDocument["D"];
  awayTarget.P = jsonDocument["aP"], awayTarget.I = jsonDocument["aI"], awayTarget.D = jsonDocument["aD"];
  eqPwr = jsonDocument["Ep"], heaterInterval = jsonDocument["hi"], pidInt = jsonDocument["pidi"],
  sensorSampleInterval = jsonDocument["ssi"];
  maxCool = jsonDocument["maxcool"];

#ifdef ENABLE_MQTT
  if (nullptr == jsonDocument["mqttHost"])
  {
    Serial.println("mqqTHost is a NULLPTR");
    strcpy(mqttHost, MQTT_HOST);
  }
  else
  {
    strcpy(mqttHost, jsonDocument["mqttHost"]);
  }

  if (nullptr == jsonDocument["mqttTopic"])
  {
    strcpy(mqttTopic, MQTT_TOPIC);
  }
  else
  {
    strcpy(mqttTopic, jsonDocument["mqttTopic"]);
  }

  if (nullptr == jsonDocument["mqttUser"])
  {
    strcpy(mqttUser, MQTT_USER);
  }
  else
  {
    strcpy(mqttUser, jsonDocument["mqttUser"]);
  }

  if (nullptr == jsonDocument["mqttPass"])
  {
    strcpy(mqttPass, MQTT_PASS);
  }
  else
  {
    strcpy(mqttPass, jsonDocument["mqttPass"]);
  }

  mqttPort = jsonDocument["mqttPort"];
  if (!mqttPort)
    mqttPort = MQTT_PORT;

#endif // ENABLE_MQTT

  WM_AP_IPconfig._ap_static_ip[0] = jsonDocument["ap_static_ip"][0];
  WM_AP_IPconfig._ap_static_ip[1] = jsonDocument["ap_static_ip"][1];
  WM_AP_IPconfig._ap_static_ip[2] = jsonDocument["ap_static_ip"][2];
  WM_AP_IPconfig._ap_static_ip[3] = jsonDocument["ap_static_ip"][3];

  WM_AP_IPconfig._ap_static_gw[0] = jsonDocument["ap_static_gw"][0];
  WM_AP_IPconfig._ap_static_gw[1] = jsonDocument["ap_static_gw"][1];
  WM_AP_IPconfig._ap_static_gw[2] = jsonDocument["ap_static_gw"][2];
  WM_AP_IPconfig._ap_static_gw[3] = jsonDocument["ap_static_gw"][3];

  WM_AP_IPconfig._ap_static_sn[0] = jsonDocument["ap_static_sn"][0];
  WM_AP_IPconfig._ap_static_sn[1] = jsonDocument["ap_static_sn"][1];
  WM_AP_IPconfig._ap_static_sn[2] = jsonDocument["ap_static_sn"][2];
  WM_AP_IPconfig._ap_static_sn[3] = jsonDocument["ap_static_sn"][3];

  WM_STA_IPconfig._sta_static_ip[0] = jsonDocument["sta_static_ip"][0];
  WM_STA_IPconfig._sta_static_ip[1] = jsonDocument["sta_static_ip"][1];
  WM_STA_IPconfig._sta_static_ip[2] = jsonDocument["sta_static_ip"][2];
  WM_STA_IPconfig._sta_static_ip[3] = jsonDocument["sta_static_ip"][3];

  WM_STA_IPconfig._sta_static_gw[0] = jsonDocument["sta_static_gw"][0];
  WM_STA_IPconfig._sta_static_gw[1] = jsonDocument["sta_static_gw"][1];
  WM_STA_IPconfig._sta_static_gw[2] = jsonDocument["sta_static_gw"][2];
  WM_STA_IPconfig._sta_static_gw[3] = jsonDocument["sta_static_gw"][3];

  WM_STA_IPconfig._sta_static_sn[0] = jsonDocument["sta_static_sn"][0];
  WM_STA_IPconfig._sta_static_sn[1] = jsonDocument["sta_static_sn"][1];
  WM_STA_IPconfig._sta_static_sn[2] = jsonDocument["sta_static_sn"][2];
  WM_STA_IPconfig._sta_static_sn[3] = jsonDocument["sta_static_sn"][3];

#if USE_CONFIGURABLE_DNS
  WM_STA_IPconfig._sta_static_dns1[0] = jsonDocument["sta_static_dns1"][0];
  WM_STA_IPconfig._sta_static_dns1[1] = jsonDocument["sta_static_dns1"][1];
  WM_STA_IPconfig._sta_static_dns1[2] = jsonDocument["sta_static_dns1"][2];
  WM_STA_IPconfig._sta_static_dns1[3] = jsonDocument["sta_static_dns1"][3];

  WM_STA_IPconfig._sta_static_dns2[0] = jsonDocument["sta_static_dns2"][0];
  WM_STA_IPconfig._sta_static_dns2[1] = jsonDocument["sta_static_dns2"][1];
  WM_STA_IPconfig._sta_static_dns2[2] = jsonDocument["sta_static_dns2"][2];
  WM_STA_IPconfig._sta_static_dns2[3] = jsonDocument["sta_static_dns2"][3];

#endif

  if (jsonDocument["WifiCredential_ssid"])
  {
    int i = 0;
    JsonArray j_ssid = jsonDocument["WifiCredential_ssid"];
    // using C++11 syntax (preferred):
    for (JsonVariant value : j_ssid)
    {
      strcpy(WM_config.WiFi_Creds[i].wifi_ssid, value.as<const char *>());
      i++;
      if (i == NUM_WIFI_CREDENTIALS)
        break;
    }
  }
  if (jsonDocument["WifiCredential_pw"])
  {
    int i = 0;
    JsonArray j_ssid = jsonDocument["WifiCredential_pw"];
    // using C++11 syntax (preferred):
    for (JsonVariant value : j_ssid)
    {
      strcpy(WM_config.WiFi_Creds[i].wifi_pw, value.as<const char *>());
      i++;
      if (i == NUM_WIFI_CREDENTIALS)
        break;
    }
  }

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
#ifdef ENABLE_MQTT
  if (mqttHost)
    jsonDocument["mqttHost"] = mqttHost;
  if (mqttTopic)
    jsonDocument["mqttTopic"] = mqttTopic;
  if (mqttUser)
    jsonDocument["mqttUser"] = mqttUser;
  if (mqttPass)
    jsonDocument["mqttPass"] = mqttPass;
  jsonDocument["mqttPort"] = mqttPort;
#endif // ENABLE_MQTT

  jsonDocument["ap_static_ip"][0] = WM_AP_IPconfig._ap_static_ip[0];
  jsonDocument["ap_static_ip"][1] = WM_AP_IPconfig._ap_static_ip[1];
  jsonDocument["ap_static_ip"][2] = WM_AP_IPconfig._ap_static_ip[2];
  jsonDocument["ap_static_ip"][3] = WM_AP_IPconfig._ap_static_ip[3];

  jsonDocument["ap_static_gw"][0] = WM_AP_IPconfig._ap_static_gw[0];
  jsonDocument["ap_static_gw"][1] = WM_AP_IPconfig._ap_static_gw[1];
  jsonDocument["ap_static_gw"][2] = WM_AP_IPconfig._ap_static_gw[2];
  jsonDocument["ap_static_gw"][3] = WM_AP_IPconfig._ap_static_gw[3];

  jsonDocument["ap_static_sn"][0] = WM_AP_IPconfig._ap_static_sn[0];
  jsonDocument["ap_static_sn"][1] = WM_AP_IPconfig._ap_static_sn[1];
  jsonDocument["ap_static_sn"][2] = WM_AP_IPconfig._ap_static_sn[2];
  jsonDocument["ap_static_sn"][3] = WM_AP_IPconfig._ap_static_sn[3];

  jsonDocument["sta_static_ip"][0] = WM_STA_IPconfig._sta_static_ip[0];
  jsonDocument["sta_static_ip"][1] = WM_STA_IPconfig._sta_static_ip[1];
  jsonDocument["sta_static_ip"][2] = WM_STA_IPconfig._sta_static_ip[2];
  jsonDocument["sta_static_ip"][3] = WM_STA_IPconfig._sta_static_ip[3];

  jsonDocument["sta_static_gw"][0] = WM_STA_IPconfig._sta_static_gw[0];
  jsonDocument["sta_static_gw"][1] = WM_STA_IPconfig._sta_static_gw[1];
  jsonDocument["sta_static_gw"][2] = WM_STA_IPconfig._sta_static_gw[2];
  jsonDocument["sta_static_gw"][3] = WM_STA_IPconfig._sta_static_gw[3];

  jsonDocument["sta_static_sn"][0] = WM_STA_IPconfig._sta_static_sn[0];
  jsonDocument["sta_static_sn"][1] = WM_STA_IPconfig._sta_static_sn[1];
  jsonDocument["sta_static_sn"][2] = WM_STA_IPconfig._sta_static_sn[2];
  jsonDocument["sta_static_sn"][3] = WM_STA_IPconfig._sta_static_sn[3];

#if USE_CONFIGURABLE_DNS
  jsonDocument["sta_static_dns1"][0] = WM_STA_IPconfig._sta_static_dns1[0];
  jsonDocument["sta_static_dns1"][1] = WM_STA_IPconfig._sta_static_dns1[1];
  jsonDocument["sta_static_dns1"][2] = WM_STA_IPconfig._sta_static_dns1[2];
  jsonDocument["sta_static_dns1"][3] = WM_STA_IPconfig._sta_static_dns1[3];

  jsonDocument["sta_static_dns2"][0] = WM_STA_IPconfig._sta_static_dns2[0];
  jsonDocument["sta_static_dns2"][1] = WM_STA_IPconfig._sta_static_dns2[1];
  jsonDocument["sta_static_dns2"][2] = WM_STA_IPconfig._sta_static_dns2[2];
  jsonDocument["sta_static_dns2"][3] = WM_STA_IPconfig._sta_static_dns2[3];

#endif

  //  JsonArray data = doc.createNestedArray("data");
  //  data.add(48.756080);
  //  data.add(2.302038);

  JsonArray ssid = jsonDocument.createNestedArray("WifiCredential_ssid");
  JsonArray pw = jsonDocument.createNestedArray("WifiCredential_pw");

  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++)
  {
    ssid.add(WM_config.WiFi_Creds[i].wifi_ssid);
    pw.add(WM_config.WiFi_Creds[i].wifi_pw);
  }

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

//
// resets only the PID configuration
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
