// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//
// MQTT integration
//
#include "ESPressoMachine.h"

#ifdef ENABLE_MQTT
#define MQTT_DEBUG

// char buf_msg[2048];
#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>
#include <WiFiClient.h>
#undef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 512
#include <PubSubClient.h>
#include "mqttinterface.h"
#include "ESPressoMachine.h"
#include "ExponentialFallback.h"
// PubSubClient::setBufferSize(512)

MQTTInterface::MQTTInterface() : espClient(), client()
{
  //  myMachine = nullptr; // inherited attribute
  client.setClient(espClient);
}

void MQTTInterface::MQTT_reconnect(EspressoConfig *myConfig)
{
  if (!client.connected())
  {
    if (connectionAttempts.mustAttempt())
    {
      Serial.print("Attempting MQTT connection...");

      String clientId = "ESP32esso_" + String(ESP_getChipId(), HEX);
      // clientId += String(random(0xffff), HEX);
      if (client.connect(clientId.c_str(), myConfig->mqttUser, myConfig->mqttPass))
      {
        Serial.println("connected");
        client.subscribe(mqttConfigTopic, 1); // We should be OK with QOS 0
        Serial.println("Subscribed to " + String(mqttConfigTopic));
        connectionAttempts.hadSuccess(true);
      }
      else
      {
        Serial.print("failed, rc=");
        Serial.println(client.state());
      }
    }
  }
}

void MQTTInterface::MQTT_callback(char *topic, byte *payload, unsigned int length, ESPressoMachine *myMachine)
{
#ifdef MQTT_DEBUG
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] '");
#endif // MQTT_DEBUG
  String msg = "";
  for (int i = 0; i < length; i++)
  {
#ifdef MQTT_DEBUG
    Serial.print((char)payload[i]);
#endif // MQTT_DEBUG
    msg += (char)payload[i];
  }
#ifdef MQTT_DEBUG
  Serial.println("'");
#endif // MQTT_DEBUG

  DynamicJsonDocument jsonDocument(CONFIG_BUF_SIZE);
  DeserializationError parsingError = deserializeJson(jsonDocument, msg);
  if (parsingError)
  {
    Serial.println("Failed to deserialize json message from MQTT");
    Serial.println(parsingError.c_str());
    return;
  }
  if (jsonDocument["targetTemp"])
  {
    myMachine->myConfig->targetTemp = jsonDocument["targetTemp"];
    Serial.println("tset: " + String(myMachine->myConfig->targetTemp));
  }
  if (jsonDocument["poweroffMode"])
  {
    if (strcasecmp(jsonDocument["powerOffMode"], "true"))
      myMachine->powerOffMode = true;
    else if (strcasecmp(jsonDocument["powerOffMode"], "false"))
      myMachine->powerOffMode = false;
    else /// I know this is not very comprehensive checking.
      myMachine->powerOffMode = false;

    Serial.print("powerOffMoade: ");
    if (myMachine->powerOffMode)
      Serial.println("true");
    else
      Serial.println("false");
  }
}

void MQTTInterface::setupMQTT(ESPressoMachine *myMachine)
{
  strcpy(mqttStatusTopic, myMachine->myConfig->mqttTopic);
  strcat(mqttStatusTopic, MQTT_STATUS_TOPIC);
  strcpy(mqttConfigTopic, myMachine->myConfig->mqttTopic);
  strcat(mqttConfigTopic, MQTT_CONFIG_TOPIC);
  Serial.println("mqtt Host:" + String(myMachine->myConfig->mqttHost) + ":" + String(myMachine->myConfig->mqttPort));

  Serial.println("mqttStatusTopic:" + String(mqttStatusTopic));

  client.setServer(myMachine->myConfig->mqttHost, myMachine->myConfig->mqttPort);
  client.setCallback(std::bind(&MQTTInterface::MQTT_callback, this,
                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, myMachine));
}

void MQTTInterface::loopMQTT(ESPressoMachine *myMachine)
{
  for (int i = 0; i < MAX_CONNECTION_RETRIES && !client.connected(); i++)
  {
    myMachine->myInterface->report("Connecting MQTT","   try: "+String(i));
    MQTT_reconnect(myMachine->myConfig);
  }
  if (client.connected())
  {
    client.loop();
    client.publish(mqttStatusTopic, myMachine->machineStatus.c_str());
  }
}

#endif // ENABLE_MQTT
