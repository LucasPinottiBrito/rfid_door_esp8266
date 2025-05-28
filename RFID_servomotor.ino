#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Servo.h> // ✅ Biblioteca do Servo

WiFiServer server(80);

// ================= RFID ==================
#define SS_PIN 15  // SDA - GPIO15 (D8)
#define RST_PIN 2  // RST - GPIO2  (D4)
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ================= Servo ==================
Servo meuServo;
#define SERVO_PIN 5  // D1 (GPIO5) - Você pode mudar se quiser

// ================= URL Webhook ==================
String serverName = "https://caio2103.app.n8n.cloud/webhook-test/RFID";

StaticJsonDocument<200> doc;

unsigned long lastReadTime = 0;
const unsigned long readDelay = 3000;  // 3 segundos

void setup() {
  WiFiManager wifiManager;
  Serial.begin(115200);
  wifiManager.autoConnect("AutoConnectAP");
  server.begin();

  SPI.begin();
  mfrc522.PCD_Init();

  // 🛠️ Configura o Servo
  meuServo.attach(SERVO_PIN);
  meuServo.write(0); // ✅ Posição inicial (trancado)
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  unsigned long currentTime = millis();
  if (currentTime - lastReadTime >= readDelay) {
    lastReadTime = currentTime;

    String cardID = getCardNumber();
    Serial.print("\n🔍 Cartão detectado: ");
    Serial.println(cardID);

    validarCartao(cardID);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}

String getCardNumber() {
  String UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    UID += String(mfrc522.uid.uidByte[i], HEX);
  }
  UID.toUpperCase();
  return UID;
}

void validarCartao(String idCartao) {
  WiFiClientSecure client;
  client.setInsecure();  // ⚠️ Desativa verificação de SSL (ok para testes)

  HTTPClient http;
  String url = serverName + "?key=" + idCartao;

  Serial.print("🌐 Enviando requisição para: ");
  Serial.println(url);

  http.begin(client, url);
  String postData = "{}";
  int httpResponseCode = http.POST(postData);

  if (httpResponseCode) {
    String response = http.getString();
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("❌ Falha ao parsear JSON: ");
      Serial.println(error.c_str());
      return;
    }

    bool myField = doc["myField"];
    Serial.print("📡 Resposta do servidor: ");
    Serial.println(response);

    if (myField) {
      Serial.println("✅ Acesso LIBERADO!");

      // 🔓 Abrir a trava (girar o servo)
      meuServo.write(90);  // 🟢 Abrir (ajuste conforme seu servo)
      Serial.println("🔓 Servo abriu.");
      delay(4000);        // Espera 10 segundos
      meuServo.write(0);   // 🔒 Fechar (volta à posição inicial)
      Serial.println("🔒 Servo fechou.");

    } else {
      Serial.println("❌ Acesso NEGADO!");
    }

  } else {
    Serial.print("⚠️ Erro na requisição. Código: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

