#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "MqttClientHelper.h"

extern MqttClientHelper mqttClient;  // declare a instância

class HttpClientHelper {
public:
    static String post(const String& url, const String& payload) {
        // Desconecta MQTT antes do HTTPS
        if (mqttClient.isConnected()) {
            Serial.println("Desconectando MQTT para usar HTTPS...");
            mqttClient.disconnect();
        }

        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;
        http.begin(client, url);
        int responseCode = http.POST(payload);
        String response = responseCode > 0 ? http.getString() : "";
        http.end();

        // Reconecta MQTT
        // mqttClient.reconnect();

        return response;
    }
};
