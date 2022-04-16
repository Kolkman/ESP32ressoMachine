#ifndef ESPressoMachine_INTERFACE_H
#define ESPressoMachine_INTERFACE_H

#include "ESPressoMachineDefaults.h"

//#ifdef ENABLE_TELNET
#include "telnetinterface.h"
//#endif
//#ifdef ENABLE_MQTT
#include "mqttinterface.h"
//#endif
//#ifdef ENABLE_HTTP
#include "webinterface.h"
//#endif


class ESPressoInterface
// the Interface can be telnet, http, and/or mqtt the parent classes will need to
// reflect that
#if defined(ENABLE_TELNET) || defined(ENABLE_HTTP) || defined(ENABLE_MQTT)
    :
#endif
#ifdef ENABLE_TELNET
    public TelnetInterface
#endif

#if defined(ENABLE_HTTP) || defined(ENABLE_MQTT)
    ,
#endif
#ifdef ENABLE_HTTP
    public WebInterface
#endif
#if defined(ENABLE_MQTT) && defined(ENABLE_HTTP)
    ,
#endif
#ifdef ENABLE_MQTT
    public MQTTInterface
#endif

{
public:
    ESPressoInterface(ESPressoMachine *);
    void serialStatus();
    void loop();
    void setup();

private:
    ESPressoMachine *myMachine;
};

#endif