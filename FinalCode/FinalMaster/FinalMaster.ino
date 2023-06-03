#include <WiFi.h>
#include <esp_now.h>
#include <SD.h>
#include <SPI.h>
#include <MFRC522.h>

// Definir pines de la tarjeta micro SD
#define SD_CS_PIN 5

// Definir pines del lector RFID
#define RST_PIN 9
#define SS_PIN 10

// Dirección MAC del esclavo
uint8_t slaveMacAddress[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC}; // Reemplaza con la dirección MAC del esclavo

// Estructura para almacenar los datos a enviar
typedef struct __attribute__((packed)) {
  int data;
} MyData;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Inicializar el objeto MFRC522

void setup() {
  // Inicializar el puerto serie
  Serial.begin(115200);

  // Inicializar la tarjeta micro SD
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta SD");
    while (1);
  }

  // Inicializar el lector RFID
  SPI.begin();
  mfrc522.PCD_Init();

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    while (1);
  }

  // Configurar la dirección MAC del esclavo
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, slaveMacAddress, sizeof(slaveMacAddress));
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Agregar el esclavo como peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar el peer");
    while (1);
  }
}

void loop() {
  // Verificar si se detectó una tarjeta RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Obtener el número UID de la tarjeta RFID
    String cardUid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      cardUid += String(mfrc522.uid.uidByte[i], HEX);
    }

    // Crear el objeto de datos y asignarle un valor
    MyData myData;
    myData.data = random(100);

    // Enviar los datos al esclavo a través de ESP-NOW
    esp_now_send(slaveMacAddress, (uint8_t *)&myData, sizeof(myData));

    // Detener la detección de la tarjeta RFID
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}
