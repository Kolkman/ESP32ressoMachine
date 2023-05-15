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
#ifdef ENABLE_OLED
#include "oledinterface.h"
#endif
#if  defined(ENABLE_OLED) && defined (ENABLE_LIQUID)
#error "Both OLED and LIQUID Interfaces defined"
#endif
#if defined(ENABLE_OLED) || defined (ENABLE_LIQUID)
#ifdef ENABLE_BUTTON
#include "buttonInterface.h"
#endif // ENABLE_BUTTON
#endif // ENABLE_LIQuID or OLED

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
#if defined(ENABLE_MQTT) && (defined(ENABLE_LIQUID) || defined(ENABLE_OLED))
    ,
#endif

#ifdef ENABLE_LIQUID
                          public LiquidInterface
#endif // ENABLE_LIQUID
#ifdef ENABLE_OLED
                          public OledInterface           
#endif //ENABLE_OLED
#if defined(ENABLE_OLED) || defined(ENABLE_LIQUID)
#ifdef ENABLE_BUTTON
        , public ButtonInterface
#endif // ENABLE_BUTTON
#endif // One of the interfaces defined
{
public:
    ESPressoInterface(ESPressoMachine *);
    ESPressoMachine * myMachine;
#ifdef ENABLE_SERIAL
    void serialStatus();
    WiFiManager *wifiMngr;
#endif
    void loop();
    void setup();
    void report(String,String);

private:
    bool wasNotConnected = true;
};

#endif