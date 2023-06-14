// Slave

#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include "FS.h"

// Configuración de la tarjeta microSD
#define SD_CS_PIN 5  // Pin del chip select de la tarjeta microSD

// Leds
masterAddress[] = {0xC8, 0xF0, 0x9E, 0x52, 0x86, 0x9C};

//

typedef struct struct_message{
  int operation;
  int usuarioOrigen;
  int usuarioDestino;
  int creditosTransferir;
} struct_message;

struct_message myData;

String dataFile = String(myData.usuarioOrigen) + ".txt";
    dataFile = SD.open(fileName, FILE_APPEND);
    if (dataFile) {
      dataFile.println(String(myData.operation) + "," + String(myData.usuarioOrigen) + "," + String(myData.usuarioDestino) + "," + String(myData.creditosTransferir));
      dataFile.close();
      Serial.println("Datos guardados en el archivo");
  } else {
    Serial.println("Error al abrir el archivo");
  }
    }
    else {
      Serial.println("Error al abrir el archivo");
    }
  }

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta microSD");
    return;
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }

    esp_now_register_recv_cb(OnRecv);


    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, masterAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error al agregar el peer");
    return;
  }
}
    




void loop() {
  // Tu código de loop aquí
}
