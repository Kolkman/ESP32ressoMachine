// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//

#include <Arduino.h>
#include "interface.h"
#include "ESPressoMachine.h"

ESPressoInterface::ESPressoInterface(ESPressoMachine *machine) : WebInterface(machine)
{

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

  setupWebSrv(this->myMachine,WEB_USER,WEB_PASS);

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
