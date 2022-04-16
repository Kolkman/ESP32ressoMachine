

#include <Arduino.h>
#include "interface.h"
#include "ESPressoMachine.h"

ESPressoInterface::ESPressoInterface(ESPressoMachine *machine)
{
  myMachine = machine;
}

void ESPressoInterface::serialStatus()
{
  #ifdef ENABLE_SERIAL
  Serial.println(myMachine->statusAsJson());
  #endif
};

void ESPressoInterface::loop()
{

serialStatus();

#ifdef ENABLE_TELNET
  loopTelnet(myMachine->statusAsJson());
#endif
#ifdef ENABLE_MQTT
  loopMQTT(myMachine->statusAsJson());
#endif
#ifdef ENABLE_HTTP
  loopWebSrv();
#endif
}

void ESPressoInterface::setup()
{
#ifdef ENABLE_TELNET
  setupTelnet();
#endif

#ifdef ENABLE_HTTP
  setupWebSrv(this->myMachine);
#endif

#ifdef ENABLE_MQTT
  setupMQTT(this->myMachine);
#endif

#ifdef ENABLE_SWITCH_DETECTION
  setupSwitch();
#endif
}
