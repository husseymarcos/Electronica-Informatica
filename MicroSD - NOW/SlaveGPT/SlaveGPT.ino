//Faltaria la situacion en la cual se saque y vuelva a poner la tarjeta. Hacer una funcion la cual identifique la tarjeta (Esta en ChatGPT)

#include <WiFi.h>
#include <esp_now.h>
#include <SD.h>

// Definir pines de la tarjeta micro SD
#define SD_CS_PIN 5

// Estructura para almacenar los datos recibidos
typedef struct __attribute__((packed)) {
  int data;
} MyData;

// Callback para recibir datos a través de ESP-NOW
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  MyData receivedData;
  memcpy(&receivedData, data, sizeof(receivedData));

  // Guardar el dato en la tarjeta micro SD
  File file1 = SD.open("/data1.txt", FILE_APPEND); //Este archivo seria el de la operaciones de la tarjeta especifica
  if (file1) {
    file1.println(receivedData.data);
    file1.close();
  }

  File file2 = SD.open("/data2.txt", FILE_APPEND);  //Este archivo seria el .log que contiene todos los registros de todas las operaciones de todas las tarjetas
  if (file2) {
    file2.println(receivedData.data);
    file2.close();
  }
}

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

  // Configurar el ESP-NOW en modo esclavo
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

  // Registrar el callback para recibir datos
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // No es necesario realizar ninguna acción en el loop del esclavo
}
