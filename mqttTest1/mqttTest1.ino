/*
   ESP8266 o ESP32 (ver las modificaciones)
   Este script sube al server un valor "aleatorio" en el topic "XJXT06" 

*/

// #include <ESP8266WiFi.h> // Para el ESP8266
#include <WiFi.h> // Para el ESP32
WiFiClient WIFI_CLIENT;
#include <PubSubClient.h>
PubSubClient MQTT_CLIENT;

// Nombre y contraseña de tu red WiFi.
const char* ssid = "Telecentro-40fe";
const char* password = "898PHFSD88L7";

// Dirección IP y puerto del broker MQTT en la nube
const char* mqtt_server = "34.229.22.136";
const int mqtt_port = 1883;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("Conectando con ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  MQTT_CLIENT.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // Continuamente comprueba si tiene conexión con MQTT,
  // en caso de que no tenga, reconecta.
  if (!MQTT_CLIENT.connected()) {
    reconnect();
  }

  MQTT_CLIENT.loop();

  // Publicar un mensaje. Publish.
  // Convierte el entero a char. DEBE ser char.
  int aleatorio = random(1,90);
  Serial.println(aleatorio);
  String aleatorioString = String(aleatorio);
  char alea[6];
  aleatorioString.toCharArray(alea, 6);

  //                   Topic / valor
  MQTT_CLIENT.publish("library/randomNumbers", alea);

  // Espera antes de Publicar otro aleatorio.
  delay(5000);
}


// Reconecta con MQTT broker
void reconnect() {
 // Loop hasta que esté conectado
  while (!MQTT_CLIENT.connected()) {
    Serial.print("Intentando conectar con MQTT...");
    // Intenta conectarse al servidor MQTT
    if (MQTT_CLIENT.connect("ESP32Client")) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(MQTT_CLIENT.state());
      Serial.println(" Intentando de nuevo en 3 segundos");
      // Espera 3 segundos antes de volver a intentar
      delay(3000);
    }
  }
}