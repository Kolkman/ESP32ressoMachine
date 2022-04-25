#ifndef ESPressoMachine_INTERFACE_H
#define ESPressoMachine_INTERFACE_H

#include "ESPressoMachineDefaults.h"
#include "webinterface.h"

//#ifdef ENABLE_TELNET
#include "telnetinterface.h"
//#endif
//#ifdef ENABLE_MQTT
#include "mqttinterface.h"
//#endif


class ESPressoInterface : public WebInterface
// the Interface can be telnet, http, and/or mqtt the parent classes will need to
// reflect that

#if defined(ENABLE_TELNET) || defined(ENABLE_MQTT)
    ,
#endif
#ifdef ENABLE_TELNET
                          public TelnetInterface
#endif
#if defined(ENABLE_MQTT) && defined(ENABLE_TELNET)
    ,
#endif
#ifdef ENABLE_MQTT
                          public MQTTInterface
#endif

{
public:
    ESPressoInterface(ESPressoMachine *);
#ifdef ENABLE_SERIAL
    void serialStatus();
#endif
    void loop();
    void setup();

private:
    ESPressoMachine *myMachine;
};

#endif