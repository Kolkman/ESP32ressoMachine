// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//

#include <Arduino.h>
#include "interface.h"
#include "ESPressoMachine.h"
#include "wifiManager.h"

ESPressoInterface::ESPressoInterface(ESPressoMachine *mach) : WebInterface(mach,WEB_USER,WEB_PASS)
{
<<<<<<< HEAD
 wifiMngr= new WiFiManager(mach->myConfig);
=======
 wifiMngr= new WiFiManager();
>>>>>>> f5e6465 (WIFIManager implementation - snapshot of compilable state (not functioning))
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
  myMachine->setMachineStatus();
#ifdef ENABLE_SERIAL
  // serialStatus(machineStatus); //bit noisy
#endif

#ifdef ENABLE_TELNET
  loopTelnet(myMachine->machineStatus);
#endif
#ifdef ENABLE_MQTT
  loopMQTT(myMachine->machineStatus);
#endif
  // This Eventloop invokes
  eventLoop();
}

void ESPressoInterface::setup()
{
  wifiMngr->setup(this->server);
<<<<<<< HEAD
  Serial.println("Wifi Manager done, following up with WebSrv");
  setupWebSrv(this->myMachine);
=======


  setupWebSrv(this->myMachine,WEB_USER,WEB_PASS);
>>>>>>> f5e6465 (WIFIManager implementation - snapshot of compilable state (not functioning))

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
