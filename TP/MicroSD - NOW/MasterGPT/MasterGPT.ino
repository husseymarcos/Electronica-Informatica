#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <esp_now.h>
#include <WiFi.h>

#define SD_CS_PIN     5 // Pin CS de la tarjeta microSD
#define BUFFER_SIZE   1024 // Tamaño del búfer de datos

// Estructura para almacenar los datos que se enviarán por ESP-NOW
typedef struct {
  uint8_t data[BUFFER_SIZE];
  size_t length;
} payload_t;

payload_t payload;

// Función para inicializar ESP-NOW
void initESPNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }
}

// Función de devolución de llamada que se ejecuta cuando se recibe un mensaje a través de ESP-NOW
void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // No se requiere acción para el maestro
}

// Función para inicializar la tarjeta microSD
void initSDCard() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta microSD");
    return;
  }
  Serial.println("Tarjeta microSD inicializada correctamente");
}

void setup() {
  Serial.begin(115200);

  // Inicializar la tarjeta microSD
  initSDCard();

  // Inicializar ESP-NOW
  initESPNow();

  // Registrar la función de devolución de llamada para los mensajes recibidos
  esp_now_register_recv_cb(onDataRecv);

  // Agregar la dirección MAC del esclavo al grupo de destinos de ESP-NOW
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, C8:F0:9E:53:05:D8, 6);
  esp_now_add_peer(&peerInfo);

  Serial.println("Listo para enviar datos por ESP-NOW");
}

void loop() {
  // Leer datos de la tarjeta microSD
  File file = SD.open("/datos.txt");
  if (file) {
    // Leer los datos en el búfer
    payload.length = file.read(payload.data, BUFFER_SIZE);

    // Enviar los datos por ESP-NOW
    esp_now_send(NULL, (uint8_t*)&payload, sizeof(payload));

    file.close();
  } else {
    Serial.println("Error al abrir el archivo en la tarjeta microSD");
  }

  delay(1000); // Esperar un segundo antes de enviar nuevos datos
}
