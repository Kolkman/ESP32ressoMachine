//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// MQTT integration
//
#include "ESPressoMachine.h"

#ifdef ENABLE_MQTT
#define MQTT_DEBUG 

// char buf_msg[2048];

#include <SPI.h>
#include <Ethernet.h>
#include <WiFiClient.h>
#undef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 512
#include <PubSubClient.h>
#include "mqttinterface.h"
#include "ESPressoMachine.h"
// PubSubClient::setBufferSize(512)

MQTTInterface::MQTTInterface() : espClient(), client()
{
  myMachine = nullptr;
  client.setClient(espClient);
}

void MQTTInterface::MQTT_reconnect()
{
  if (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP8266Client";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
    {
      Serial.println("connected");
      client.subscribe(mqttConfigTopic, 1); // We should be OK with QOS 0 
      Serial.println("Subscribed to "+String(mqttConfigTopic));
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }
}

void MQTTInterface::MQTT_callback(char *topic, byte *payload, unsigned int length)
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
Serial.println("Digging into ones data");
Serial.println(myMachine->myConfig->targetTemp);

  double val = msg.toFloat();
  Serial.println(val);
  bool reconf=false;
  if (strstr(topic, "/config/tset"))
  {
    if (val > 1e-3)
      myMachine->myConfig->targetTemp = val;
      reconf=true;
      Serial.println("targetTemp set to "+String(val));
  }
  else if (strstr(topic, "/config/toggle"))
  {
    myMachine->poweroffMode = (!myMachine->poweroffMode);    
  }
  if (reconf){
    myMachine->reConfig();
  }
}

void MQTTInterface::setupMQTT(ESPressoMachine *machine)
{
  myMachine = machine;
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(std::bind(&MQTTInterface::MQTT_callback, this,
                               std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void MQTTInterface::loopMQTT(String jsonstring)
{

  for (int i = 0; i < MAX_CONNECTION_RETRIES && !client.connected(); i++)
  {
    MQTT_reconnect();
    Serial.print(".");
    MQTT_reconnect();
  }

  client.loop();
  client.publish(mqttStatusTopic, jsonstring.c_str());
}

#endif // ENABLE_MQTT