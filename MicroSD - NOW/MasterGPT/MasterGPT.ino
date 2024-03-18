#include <WiFi.h>
#include <esp_now.h>

// Dirección MAC del esclavo
uint8_t slaveMacAddress[] = {0xC8, 0xF0, 0x9E, 0x53, 0x05, 0xD8};

// Estructura para almacenar los datos a enviar
typedef struct __attribute__((packed)) {
  int data; //La idea es mandar ints, con un sistema propio para identificar las operaciones efectuadas
} MyData;

void setup() {
  // Inicializar el puerto serie
  Serial.begin(115200);

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
  // Esperar un poco antes de enviar datos
  delay(2000);

  // Crear el objeto de datos y asignarle un valor
  MyData myData;
  myData.data = random(100);  //Aca habria que unir el codigo del keypad asi dependiendo lo que toque el usuario que datos se mandan a la micro sd

  // Enviar los datos al esclavo a través de ESP-NOW
  esp_now_send
