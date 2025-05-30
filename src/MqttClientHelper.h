#ifndef MQTT_CLIENT_HELPER_H
#define MQTT_CLIENT_HELPER_H

#include <PubSubClient.h>
#include <WiFiClientSecure.h>

class MqttClientHelper {
public:
    MqttClientHelper(const char* host, int port, const char* user = nullptr, const char* pass = nullptr);
    void setup(const char* clientId);
    void loop();
    bool publish(const char* topic, const char* payload);
    bool isConnected();

private:
    const char* mqtt_host;
    int mqtt_port;
    const char* mqtt_user;
    const char* mqtt_pass;
    WiFiClientSecure wifiClient;
    PubSubClient mqttClient;
    const char* mqtt_clientId;
    void reconnect();
};

#endif
