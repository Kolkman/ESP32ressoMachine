#ifndef ESPRressMach_MQTT_H
#define ESPRressMach_MQTT_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "ESPressoMachineDefaults.h"
#include "ESPressoMachine.h"



class MQTTInterface
{
public:
    MQTTInterface();
    void setupMQTT(ESPressoMachine *);
    void loopMQTT(String);


private:
//    ESPressoMachine *myMachine;
    WiFiClient espClient;
    PubSubClient client;
    void MQTT_reconnect();
    void MQTT_callback(char*, byte*, unsigned int, ESPressoMachine *);
    char mqttStatusTopic[256+MQTT_TOPIC_EXT_LENGTH];
    char mqttConfigTopic[256+MQTT_TOPIC_EXT_LENGTH];
};
#endif
