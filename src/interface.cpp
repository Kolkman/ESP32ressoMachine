// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//

#include <Arduino.h>
#include "interface.h"
#include "ESPressoMachine.h"
#include "wifiManager.h"

ESPressoInterface::ESPressoInterface(ESPressoMachine *mach) : WebInterface(mach, WEB_USER, WEB_PASS)
{
  wifiMngr = new WiFiManager();
}

void ESPressoInterface::serialStatus()
{
#ifdef ENABLE_SERIAL
  myMachine->setMachineStatus();
  Serial.println(myMachine->machineStatus);
#endif
};

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
  // serialStatus(machineStatus); //bit noisy
#endif

#ifdef ENABLE_TELNET
  loopTelnet(myMachine->machineStatus);
#endif
#ifdef ENABLE_MQTT
  loopMQTT(myMachine);
#endif
  // This Eventloop invokes
  eventLoop();
}

void ESPressoInterface::setup()
{
// We set this for later. Wnen there are no credentials set we want to keep the captive portal open - ad infinitum
  _waitingForClientAction = true;
  for (int i = 0; i < NUM_WIFI_CREDENTIALS; i++){

    if (strlen(myMachine->myConfig->WM_config.WiFi_Creds[i].wifi_ssid)>0)
      {
        _waitingForClientAction = false;
      }
  }
  if (_waitingForClientAction) LOGINFO("NO WiFi NEtworks set, we'll later keep the captive portal open");

  wifiMngr->setupWiFiAp(&myMachine->myConfig->WM_AP_IPconfig);
  server->reset();
  setConfigPortalPages();

  server->begin(); /// Webserver is now running....

  LOGINFO("Wifi Manager done, following up with WebSrv");
  wifiMngr->loopPortal(this); /// Wait the configuration to be finished or timed out.
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
