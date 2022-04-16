#ifndef ESPRressMach_MQTT_H
#define ESPRressMach_MQTT_H
#include "WiFiSecrets.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "ESPressoMachineDefaults.h"
#include "ESPressoMachine.h"

#define stringconcat(first, second) first second
#ifndef MQTT_TOPIC 
    #define MQTT_TOPIC "EspressoMach"
#endif
class MQTTInterface
{
public:
    MQTTInterface();
    void setupMQTT(ESPressoMachine *);
    void loopMQTT(String);
    ESPressoMachine *myMachine;

private:
    WiFiClient espClient;
    PubSubClient client;
    void MQTT_reconnect();
    void MQTT_callback(char*, byte*, unsigned int);
    
//    void (MQTTInterface::* MQTT_callbackptr) (char*, byte*, unsigned int);
    const char * const mqttStatusTopic = stringconcat(MQTT_TOPIC, "/status");
    const char * const mqttConfigTopic = stringconcat(MQTT_TOPIC, "/config");
};
#endif