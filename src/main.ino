#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Servo.h>
#include <ArduinoJson.h>
#include "RfidUtils.h"
#include "DoorControl.h"
#include "HttpRequest.h"

#define SS_PIN 15
#define RST_PIN 2
#define SERVO_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo doorServo;

unsigned long lastReadTime = 0;
const unsigned long readDelay = 3000; // 3 seconds

StaticJsonDocument<200> jsonDoc;

void setup() {
  WiFiManager wifiManager;
  wifiManager.autoConnect("ConfigAP_DoorLock");

  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  doorServo.attach(SERVO_PIN);
  lockDoor(doorServo);
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  unsigned long currentTime = millis();
  if (currentTime - lastReadTime >= readDelay) {
    lastReadTime = currentTime;

    String cardID = getCardID(mfrc522);
    Serial.print("\n🔍 Card detected: ");
    Serial.println(cardID);

    if (validateCard(cardID, jsonDoc)) {
      unlockDoor(doorServo);
      Serial.println("🔓 Door unlocked.");
      delay(4000);
      lockDoor(doorServo);
      Serial.println("🔒 Door locked.");
    } else {
      Serial.println("❌ Access denied.");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}
