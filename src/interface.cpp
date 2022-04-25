// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//

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
#ifdef ENABLE_SERIAL
  serialStatus();
#endif

#ifdef ENABLE_TELNET
  loopTelnet(myMachine->statusAsJson());
#endif
#ifdef ENABLE_MQTT
  loopMQTT(myMachine->statusAsJson());
#endif

}

void ESPressoInterface::setup()
{

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
