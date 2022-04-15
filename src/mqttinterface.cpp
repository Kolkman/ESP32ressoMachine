//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// MQTT integration
//
#include "ESPressiot.h"




#ifdef ENABLE_MQTT

//char buf_msg[2048];

#include <SPI.h>
#include <Ethernet.h>
#include <WiFiClient.h>
#undef  MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 512
#include <PubSubClient.h>
#include "mqtt.h"

//PubSubClient::setBufferSize(512)

WiFiClient espClient;
PubSubClient client(espClient);



#define stringconcat(first, second) first second
const char *mqttConfigTopic = stringconcat(MQTT_TOPIC, "/config/#");
const char *mqttStatusTopic = stringconcat(MQTT_TOPIC, "/status");


void MQTT_reconnect() {
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP8266Client";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      client.subscribe(mqttConfigTopic, 0); //We should be OK with QOS 0
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
    }
  }
}

void MQTT_callback(char* topic, byte* payload, unsigned int length) {
#ifdef MQTT_DEBUG
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] '");
#endif //MQTT_DEBUG
  String msg = "";
  for (int i = 0; i < length; i++) {
#ifdef MQTT_DEBUG
    Serial.print((char)payload[i]);
#endif //MQTT_DEBUG
    msg += (char) payload[i];
  }
#ifdef MQTT_DEBUG
  Serial.println("'");
#endif //MQTT_DEBUG

  double val = msg.toFloat();
  Serial.println(val);

  if (strstr(topic, "/config/tset")) {
    if (val > 1e-3) gTargetTemp = val;
  }
  else if (strstr(topic, "/config/toggle")) {
    poweroffMode = (!poweroffMode);
  }

}

void setupMQTT() {
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(MQTT_callback);
}

void loopMQTT(String jsonstring) {

  for (int i = 0; i < MAX_CONNECTION_RETRIES && !client.connected(); i++) {
    MQTT_reconnect();
    Serial.print(".");
    MQTT_reconnect();
  }

  client.loop();
  client.publish(mqttStatusTopic, jsonstring.c_str());
}

#endif // ENABLE_MQTT