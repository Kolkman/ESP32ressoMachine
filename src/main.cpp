//
// ESPressIoT Controller for Espresso Machines
// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//

#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <PID_v1.h>

// local Includes
#include "ESPressoMachine.h"
#include "WiFiSecrets.h"
#include "config.h"
#include "helpers.h"

#include "telnetinterface.h"
#include "webinterface.h"
#include "mqttinterface.h"
#include "pidtuner.h"

ESPressoMachine  myRancilio;


uint8_t mac[6];

void setup()
{
  Serial.begin(115200);
  // Need to print something to get the serial monitor setled
  for (int i = 0; i < 5; i++)
  {
    Serial.print("Initializing (");
    Serial.print(i);
    Serial.println(")");
    delay(100);
  }



  Serial.println("Mounting SPIFFS...");
  if (!myRancilio.myConfig->prepareFS())
  {
    Serial.println("Failed to mount SPIFFS !");
  }
  else
  {
    Serial.println("Mounted.");
  }

  Serial.println("Loading config...");
  if (!myRancilio.myConfig->loadConfig())
  {
    Serial.println("Failed to load config. Using default values and creating config...");
    if (!myRancilio.myConfig->saveConfig())
    {
      Serial.println("Failed to save config");
    }
    else
    {
      Serial.println("Config saved");
    }
  }
  else
  {
    Serial.println("Config loaded");
  }

  Serial.print("Firmware Version");
  Serial.println(String(CURRENTFIRMWARE) +" "+ String(F(__DATE__))+":"+String(F(__TIME__)));
  Serial.println("Settin up PID...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.macAddress(mac);
  Serial.println("");
  Serial.print("MAC address: ");
  Serial.println(macToString(mac));

  Serial.print("Connecting to Wifi AP");
  for (int i = 0; i < MAX_CONNECTION_RETRIES && WiFi.status() != WL_CONNECTED; i++)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Error connection to AP after ");
    Serial.print(MAX_CONNECTION_RETRIES);
    Serial.println(" retries.");
    throw("Could not connect to WIFI");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(2);

  // setup the Machine

  // start PID
  myRancilio.startMachine();
  myRancilio.manageTemp();
  
}

void loop()
{
  if (myRancilio.heatLoop()) {
      myRancilio.myInterface->loop();
  }
}