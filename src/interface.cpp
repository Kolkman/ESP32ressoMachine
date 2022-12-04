// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//

#include <Arduino.h>
#include "interface.h"
#include "ESPressoMachine.h"
#include "wifiManager.h"

ESPressoInterface::ESPressoInterface(ESPressoMachine *mach) : WebInterface(mach, WEB_USER, WEB_PASS)
{
  myMachine = mach;
  wifiMngr = new WiFiManager(this);
}

void ESPressoInterface::serialStatus()
{
#ifdef ENABLE_SERIAL
  myMachine->setMachineStatus();
  Serial.println(myMachine->machineStatus);
#endif
};

void ESPressoInterface::report(String firstInput = "", String secondInput = "")
{
  LOGINFO1("Report1: ", firstInput);
  LOGINFO1("Report2: ", secondInput);
#ifdef ENABLE_LIQUID
  if (!backlightIsOn)
  {
    lcd->displayOn();
    backlightIsOn = true;
  }
  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print(firstInput);
  lcd->setCursor(0, 2);
  lcd->print(secondInput);
#endif
}

void ESPressoInterface::loop()
{
  if (wifiMngr->run(WIFI_MULTI_CONNECT_WAITING_MS) == WL_CONNECTED)
  {
    if (wasNotConnected)
    {
      LOGERROR(F("WiFi (re)connected"));
      LOGERROR3(F("SSID:"), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
      LOGERROR3(F("Channel:"), WiFi.channel(), F(",IP address:"), WiFi.localIP());
      wasNotConnected = false;
      int n = WiFi.scanComplete();
      if (n == -2)
      {
        WiFi.scanDelete();
        // Just to have some resuls when needed.
        LOGERROR("Scanning started");
        WiFi.scanNetworks(true);
      }
    }
  }
  else
  {
    Serial.println("WiFi not connected!");
    wasNotConnected = true;
  }

  myMachine->setMachineStatus();
#ifdef ENABLE_SERIAL
  //  serialStatus(); //bit noisy
#endif
#ifdef ENABLE_LIQUID
  loopLiquid(myMachine);
#endif
#ifdef ENABLE_TELNET
  loopTelnet(myMachine->machineStatus);
#endif
#ifdef ENABLE_MQTT
  loopMQTT(myMachine);
#endif

  // This  invokes the Eventloop that puts info on the websocket
  eventLoop();
}

void ESPressoInterface::setup()
{
  bool _initConfig = true;

#ifdef ENABLE_LIQUID
  setupLiquid();
#ifdef ENABLE_BUTTON
  _initConfig = setupButton(this->myMachine); // use the button interface to initiate the stand alone
                                              // configuration
#endif                                        // ENABLE_BUTTON
#endif                                        // ENABLE_LIQUID

  // We set this for later. Wnen there are no credentials set we want to keep the captive portal open - ad infinitum
  _waitingForClientAction = true;
  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++)
  {

    if (strlen(myMachine->myConfig->WM_config.WiFi_Creds[i].wifi_ssid) > 0)
    {
      _waitingForClientAction = false;
    }
  }
  if (_waitingForClientAction)
    LOGINFO("NO WiFi NEtworks set, we'll later keep the captive portal open");
  // Config cycle only happens if the button is pressed 
  if (_initConfig)
  {
    wifiMngr->setupWiFiAp(&myMachine->myConfig->WM_AP_IPconfig);
    server->reset();
    setConfigPortalPages();
    server->begin(); /// Webserver is now running....
    LOGINFO("Wifi Manager done, following up with WebSrv");
    wifiMngr->loopPortal(); /// Wait the configuration to be finished or timed out.
  }
  /// Configuration should now be set.
  wifiMngr->connectMultiWiFi(myMachine->myConfig);
  server->reset();

  setupWebSrv(this->myMachine);

#ifdef ENABLE_TELNET
  setupTelnet();
#endif

#ifdef ENABLE_MQTT
  setupMQTT(this->myMachine);
#endif

#ifdef ENABLE_SWITCH_DETECTION
  setupSwitch();
#endif
}
