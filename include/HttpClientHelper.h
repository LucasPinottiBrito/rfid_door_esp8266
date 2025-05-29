#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

class HttpClientHelper {
public:
    static String post(const String& url, const String& payload) {
        WiFiClientSecure client;
        client.setInsecure();

        HTTPClient http;
        http.begin(client, url);
        int responseCode = http.POST(payload);
        String response = responseCode > 0 ? http.getString() : "";
        http.end();
        return response;
    }
};
