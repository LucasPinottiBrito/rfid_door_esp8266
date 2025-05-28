#ifndef HTTP_CLIENT_HELPER_H
#define HTTP_CLIENT_HELPER_H

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

enum HttpMethod {
  GET,
  POST,
  PUT,
  DELETE
};

template <size_t CAPACITY>
bool sendHttpRequest(
  const String& url,
  HttpMethod method,
  const String& payload,
  StaticJsonDocument<CAPACITY>& jsonDoc,
  bool useSecure = true
) {
  WiFiClientSecure secureClient;
  WiFiClient plainClient;
  HTTPClient http;

  if (useSecure) secureClient.setInsecure();

  bool started = useSecure
    ? http.begin(secureClient, url)
    : http.begin(plainClient, url);

  if (!started) return false;

  int httpCode;
  if (method == POST)
    httpCode = http.POST(payload);
  else if (method == PUT)
    httpCode = http.sendRequest("PUT", payload);
  else if (method == DELETE)
    httpCode = http.sendRequest("DELETE");
  else
    httpCode = http.GET();

  if (httpCode <= 0) {
    http.end();
    return false;
  }

  String response = http.getString();
  DeserializationError error = deserializeJson(jsonDoc, response);
  http.end();

  return !error;
}

#endif
