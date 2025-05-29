#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

#include "StepperController.h"
#include "HttpClientHelper.h"
#include "WiFiManagerHelper.h"
#include "RfidUtils.h"

#define SS_PIN 15  // SDA
#define RST_PIN 2  // RST
#define SERVO_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);
StepperController stepper(SERVO_PIN);
StaticJsonDocument<200> doc;

const String serverUrl = "https://caio2103.app.n8n.cloud/webhook-test/RFID";
unsigned long lastReadTime = 0;
const unsigned long readDelay = 3000;

void setup() {
    Serial.begin(115200);
    WiFiManagerHelper::connect();
    SPI.begin();
    rfid.PCD_Init();
    stepper.begin();
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

    unsigned long now = millis();
    if (now - lastReadTime < readDelay) return;
    lastReadTime = now;

    String cardID = RfidUtils::getCardNumber(rfid);
    Serial.println("🔍 Card detected: " + cardID);

    String response = HttpClientHelper::post(serverUrl + "?key=" + cardID, "{}");

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        Serial.println("❌ JSON parse failed: " + String(error.c_str()));
        return;
    }

    bool accessGranted = doc["myField"];
    if (accessGranted) {
        Serial.println("✅ Access GRANTED!");
        stepper.unlock();
        delay(4000);
        stepper.lock();
    } else {
        Serial.println("❌ Access DENIED!");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
