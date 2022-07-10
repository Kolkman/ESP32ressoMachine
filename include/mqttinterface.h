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



#ifndef MQTT_STATUS_TOPIC 
    #define MQTT_STATUS_TOPIC "/status"
#endif

#ifndef MQTT_CONFIG_TOPIC 
    #define MQTT_CONFIG_TOPIC "/config"
#endif

// Max length of the strings above.
#ifndef MQTT_TOPIC_EXT_LENGTH
    #define MQTT_TOPIC_EXT_LENGTH 7
#endif


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
