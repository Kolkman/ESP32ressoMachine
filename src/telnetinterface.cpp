//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Telnet Server
//
#include "ESPressoMachine.h"
#include "WIFI.h"
//#include "helpers.h"

#ifdef ENABLE_TELNET
#include <telnetinterface.h>

TelnetInterface::TelnetInterface() : telnetServer(23), telnetClient()
{
  // empty
}

void TelnetInterface::telnetStatus(String statusAsJson)
{
  if (!telnetClient)
    return; // error condition
  telnetClient.println(statusAsJson);
}

void TelnetInterface::setupTelnet()
{

  telnetServer.begin();
  telnetServer.setNoDelay(true);

  Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");
  Serial.print("Free Heap[B]: ");
  Serial.println(ESP.getFreeHeap());
}

void TelnetInterface::loopTelnet(String reportstring)
{
  if (telnetServer.hasClient())
  {
    if (!telnetClient || !telnetClient.connected())
    { // short circuit when NULL.
      if (telnetClient)
        telnetClient.stop();

      telnetClient = telnetServer.available();
    }
    else
    {
      telnetServer.available().stop();
    }
  }
  if (telnetClient && telnetClient.connected() && telnetClient.available())
  {
    telnetStatus(reportstring);
  }
}

#endif