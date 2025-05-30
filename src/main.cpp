#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

#include "StepperController.h"
#include "HttpClientHelper.h"
#include "WiFiManagerHelper.h"
#include "RfidUtils.h"

#include <ESP8266WiFi.h>
#include "MqttClientHelper.h"

#define SS_PIN 15  // SDA
#define RST_PIN 2  // RST
#define SERVO_PIN 5

MqttClientHelper mqttClient(
    "e519d15084bd448286ec5dd6851ab338.s2.eu.hivemq.cloud", // host
    8883,                 // port
    "Rfid_device",      // user
    "Rfid_device1"      
);
const String device_id = "e00001";

MFRC522 rfid(SS_PIN, RST_PIN);
StepperController stepper(SERVO_PIN);
StaticJsonDocument<200> doc;

const String serverUrl = "https://lucaspinotti.app.n8n.cloud/webhook/RFID";
unsigned long lastReadTime = 0;
const unsigned long readDelay = 3000;

void setup() {
    Serial.begin(115200);
    WiFiManagerHelper::connect();
    SPI.begin();
    rfid.PCD_Init();
    stepper.begin();
    
    mqttClient.setup(device_id.c_str());
    
    mqttClient.setCallback([](char* topic, byte* payload, unsigned int length) {
        Serial.print("Mensagem recebida no tópico: ");
        Serial.println(topic);
        Serial.print("Conteúdo: ");
        for (unsigned int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();

        if (strcmp(topic, "/smartroom/porta/control") == 0) {
            if ((char)payload[0] == '1') {
                Serial.println("Abrindo porta!");
                stepper.unlock();
                mqttClient.publish("/smartroom/log", (device_id + "/unlock").c_str());
            }
            else if ((char)payload[0] == '0') {
                Serial.println("Fechando porta!");
                stepper.lock();
                mqttClient.publish("/smartroom/log", (device_id + "/lock").c_str());
            } else {
                Serial.println("Comando desconhecido recebido.");
            }
        }
    });
}

void loop() {
    mqttClient.loop();
    
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

    unsigned long now = millis();
    if (now - lastReadTime < readDelay) return;
    lastReadTime = now;

    String cardID = RfidUtils::getCardNumber(rfid);
    Serial.println("Card detected: " + cardID);

    String response = HttpClientHelper::post(serverUrl + "?key=" + cardID, "{}");

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
        Serial.println("JSON parse failed: " + String(error.c_str()));
        return;
    }

    bool accessGranted = doc["myField"];
    if (accessGranted) {
        Serial.println("Access GRANTED!");
        stepper.unlock();
        mqttClient.publish("/smartroom/log", (device_id + "/unlock").c_str());
        delay(4000);
        stepper.lock();
    } else {
        Serial.println("Access DENIED!");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
