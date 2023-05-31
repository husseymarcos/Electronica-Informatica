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

// Función de devolución de llamada que
void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // Escribir los datos recibidos en la tarjeta microSD
  File file = SD.open("/datos_recibidos.txt", FILE_WRITE);
  if (file) {
    file.write(data, data_len);
    file.close();
    Serial.println("Datos recibidos y escritos en la tarjeta microSD");
  } else {
    Serial.println("Error al abrir el archivo en la tarjeta microSD");
  }
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

  // Configurar el dispositivo como esclavo de ESP-NOW
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

  Serial.println("Listo para recibir datos por ESP-NOW");
}

void loop() {
  // No se requiere acción en el bucle principal para el esclavo
}

