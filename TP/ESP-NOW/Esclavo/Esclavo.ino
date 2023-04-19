// ESCLAVO (recibe datos del maestro)


#include <esp_now.h>
#include <WiFi.h>
// Estructura de datos que va a contener el mensaje recibido
// requerido, debe ser igual al del maestro

struct aRecibir {
  char palabra[6];
  int numero;
};
// crea una estructura para recibir los datos
aRecibir Datos;

// Funcion que se ejecuta cuando se recibe un mensaje
void OnRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&Datos, incomingData, sizeof(Datos));
  Serial.print("Bytes recibidos: ");
  Serial.println(len);
  Serial.print("Datos recibidos : ");
  Serial.println(Datos.palabra);
  Serial.println(Datos.numero);
}
void setup() {
  // inicializo Serial Monitor
  Serial.begin(115200);
  // Seteo placa como estacion Wi-Fi
  WiFi.mode(WIFI_STA);
  // Inicializo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }
  // Despues de inicializar el protocolo ESP-Now registro la funcion que recibe los mensajes
  // para poder ejecutarla cuando llega un mensaje.
  esp_now_register_recv_cb(OnRecv);
}

void loop() {
}
Footer
© 2023 GitHub, Inc.