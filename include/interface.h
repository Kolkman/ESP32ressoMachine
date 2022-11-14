#ifndef ESPressoMachine_INTERFACE_H
#define ESPressoMachine_INTERFACE_H
#include "ESPressoMachineDefaults.h"
#include "wifiManager.h"
#include "webinterface.h"
//#ifdef ENABLE_TELNET
#include "telnetinterface.h"
//#endif
//#ifdef ENABLE_MQTT
#include "mqttinterface.h"
//#endif
#ifdef ENABLE_LIQUID
#include "liquidinterface.h"
#endif

class WiFiManager; // forward declaration

class ESPressoInterface : public WebInterface
// the Interface can be telnet, http, and/or mqtt the parent classes will need to
// reflect that

#if defined(ENABLE_TELNET) || defined(ENABLE_MQTT) || defined(ENABLE_LIQUID)
    ,
#endif
#ifdef ENABLE_TELNET
                          public TelnetInterface
#endif
#if defined(ENABLE_TELNET) && (defined(ENABLE_MQTT) || defined(ENABLE_LIQUID))
    ,
#endif
#ifdef ENABLE_MQTT
                          public MQTTInterface
#endif
#if defined(ENABLE_MQTT) && defined(ENABLE_LIQUID)
    ,
#endif
#ifdef ENABLE_LIQUID
                          public LiquidInterface
#endif
{
public:
    ESPressoInterface(ESPressoMachine *);
#ifdef ENABLE_SERIAL
    void serialStatus();
    WiFiManager *wifiMngr;
#endif
    void loop();
    void setup();

private:
    bool wasNotConnected = true;
};

#endif