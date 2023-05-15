//
// ESPressIoT Controller for Espresso Machines
// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//

#include <Arduino.h>
//#include <WiFi.h>

// local Includes
#include "ESPressoMachine.h"

#include "config.h"
#include "helpers.h"

#include "telnetinterface.h"
#include "webinterface.h"
#ifdef ENABLE_MQTT
#include "mqttinterface.h"
#endif // ENABLE_MQTT

#ifdef ENABLE_BUTTON
#include "buttonInterface.h"
#endif

#include "pidtuner.h"

ESPressoMachine myRancilio;

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

  Serial.println("Mounting LittleFS...");
  if (!myRancilio.myConfig->prepareFS())
  {
    Serial.println("Failed to mount LittleFS !");
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
  Serial.println(String(CURRENTFIRMWARE) + " " + String(F(__DATE__)) + ":" + String(F(__TIME__)));
  Serial.println("Setting up PID Control...");

  // setup the Machine

  // start PID
  myRancilio.startMachine();
  myRancilio.manageTemp();
}

void loop()
{

  if (myRancilio.heatLoop())
  {
    // Only send out information when there is something new to report
    myRancilio.myInterface->loop();
  }
#ifdef ENABLE_BUTTON
  myRancilio.myInterface->loopButton(&myRancilio);
#endif // ENABLE_BUTTON
}
