#include <WiFi.h>
#include <esp_now.h>
#include <SD.h>

// Definir pines de la tarjeta micro SD
#define SD_CS_PIN 5

// Estructura para almacenar los datos recibidos
typedef struct __attribute__((packed)) {
  int data;
} MyData;

void setup() {
  // Inicializar el puerto serie
  Serial.begin(115200);

  // Inicializar la tarjeta micro SD
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta SD");
    while (1);
  }

  // Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    while (1);
  }

  // Configurar ESP-NOW en modo esclavo
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

  // Registrar el callback para recibir datos
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // No es necesario realizar ninguna acción en el loop del esclavo
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // Callback para recibir datos a través de ESP-NOW
  MyData receivedData;
  memcpy(&receivedData, data, sizeof(receivedData));

  // Guardar el dato en la tarjeta micro SD
  File file = SD.open("/data.txt", FILE_APPEND);
  if (file) {
    file.println(receivedData.data);
    file.close();
  }
}
