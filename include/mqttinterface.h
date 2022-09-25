

#ifndef ESPRressMach_MQTT_H
#define ESPRressMach_MQTT_H
#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "ESPressoMachineDefaults.h"
#include "ESPressoMachine.h"
#include "ExponentialFallback.h"
#ifdef ENABLE_MQTT



class MQTTInterface
{
public:
    MQTTInterface();
    void setupMQTT(ESPressoMachine *);
    void loopMQTT(ESPressoMachine *);


private:
//    ESPressoMachine *myMachine;
    WiFiClient espClient;
    PubSubClient client;
    void MQTT_reconnect(EspressoConfig *);
    void MQTT_callback(char*, byte*, unsigned int, ESPressoMachine *);
    char mqttStatusTopic[256+MQTT_TOPIC_EXT_LENGTH];
    char mqttConfigTopic[256+MQTT_TOPIC_EXT_LENGTH];
    ExpFallback connectionAttempts;
};
#endif //ESPRessoMach_MQTT_H
#endif //ENABLE_MQTT
