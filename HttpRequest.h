#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <ArduinoJson.h>
#include "HttpClientHelper.h"

String serverUrl = "https://caio2103.app.n8n.cloud/webhook-test/RFID";

// Validates card ID via HTTP POST request
bool validateCard(const String& cardId, StaticJsonDocument<200>& jsonDoc) {
  String url = serverUrl + "?key=" + cardId;
  String payload = "{}";

  Serial.print("🌐 Sending request to: ");
  Serial.println(url);

  bool success = sendHttpRequest(url, POST, payload, jsonDoc);

  if (!success) {
    Serial.println("⚠️ HTTP request or JSON parsing failed.");
    return false;
  }

  bool authorized = jsonDoc["myField"];
  Serial.print("📡 Server response: ");
  serializeJson(jsonDoc, Serial);
  Serial.println();

  return authorized;
}

#endif
