#include "MqttClientHelper.h"

MqttClientHelper::MqttClientHelper(const char* host, int port, const char* user, const char* pass)
    : mqtt_host(host), mqtt_port(port), mqtt_user(user), mqtt_pass(pass), mqttClient(wifiClient), mqtt_clientId(nullptr) {
    mqttClient.setServer(mqtt_host, mqtt_port);
}

void MqttClientHelper::setup(const char* clientId) {
    mqtt_clientId = clientId;
    mqttClient.setServer(mqtt_host, mqtt_port);
    reconnect();
}

void MqttClientHelper::loop() {
    if (!mqttClient.connected()) {
        reconnect();
    }
    mqttClient.loop();
}

bool MqttClientHelper::publish(const char* topic, const char* payload) {
    return mqttClient.publish(topic, payload);
}

bool MqttClientHelper::isConnected() {
    return mqttClient.connected();
}

void MqttClientHelper::reconnect() {
    wifiClient.setInsecure();
    while (!mqttClient.connected()) {
        Serial.print("Tentando conectar ao MQTT como ");
        Serial.println(mqtt_clientId);

        bool success;

        if (mqtt_user != nullptr && mqtt_pass != nullptr) {
            success = mqttClient.connect(mqtt_clientId, mqtt_user, mqtt_pass);
        } else {
            success = mqttClient.connect(mqtt_clientId);
        }

        if (success) {
            Serial.println("Conectado ao broker MQTT com sucesso!");
        } else {
            Serial.print("Falha na conexão. rc=");
            Serial.print(mqttClient.state());
            Serial.println(" Tentando novamente em 5 segundos...");
            delay(5000);
        }
    }
}
