/*
   ESP8266 o ESP32 (ver las modificaciones)
   Este script sube al server un valor "aleatorio" en el topic "XJXT06" 

*/

/*
   Para ESP32

   Modulo
   RFID RC522     ESP32

   3.3v           3.3v
   RST            27
   GND            GND
   IRQ            no se usa
   MISO           19
   MOSI           23
   SCK            18
   SDA            15

*/

// #include <ESP8266WiFi.h> // Para el ESP8266
#include <WiFi.h> // Para el ESP32
WiFiClient WIFI_CLIENT;
#include <PubSubClient.h>
PubSubClient MQTT_CLIENT;

#include "SPI.h"
#include "MFRC522.h"

#define RST_PIN  27
#define SS_PIN   15

MFRC522 mfrc522(SS_PIN, RST_PIN); 

// Nombre y contraseña de tu red WiFi.
const char* ssid = "Telecentro-40fe";
const char* password = "898PHFSD88L7";

void setup() {
  Serial.begin(115200);
  while(!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
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
}

void loop() {
  // Continuamente comprueba si tiene conexión con MQTT,
  // en caso de que no tenga, reconecta.
  if (!MQTT_CLIENT.connected()) {
    reconnect();
  }

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) { // Verificamos si hay una tarjeta RFID presente
    Serial.println("Contenido de la tarjeta:"); // Imprimimos un mensaje en el monitor serial
    String uuid = ""; // Creamos una cadena para almacenar el UUID
    for (byte i = 0; i < mfrc522.uid.size; i++) { // Recorremos cada byte del UUID
      if (mfrc522.uid.uidByte[i] < 0x10) uuid += "0"; // Si el byte es menor que 0x10, agregamos un 0 a la cadena (formato hexadecimal)
      uuid += String(mfrc522.uid.uidByte[i], HEX); // Convertimos el byte a hexadecimal y lo agregamos a la cadena UUID
    }
    Serial.println(uuid); // Imprimimos el UUID en el monitor serial

    char uuidCharArray[10];
    uuid.toCharArray(uuidCharArray, 10);
    //MQTT_CLIENT.publish("library/registerUsers", uuidCharArray);


    // Topic with the result of the query with the current card.
    //MQTT_CLIENT.subscribe("library/usersVerification"); 

    //MQTT_CLIENT.publish("library/registerUsers", uuidCharArray);
    MQTT_CLIENT.publish("library/usersVerification", uuidCharArray);

    // Parte de la comunicación desde la pagina web
    // MQTT_CLIENT.setCallback(callback);
    
    mfrc522.PICC_HaltA(); // Detenemos la comunicación con la tarjeta RFID
    mfrc522.PCD_StopCrypto1(); // Detenemos la encriptación
  }
  delay(2000);
}

// Posible código que permita informar desde la página que es lo que ocurre

/*

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';
  Serial.println(msg);

  if (String(topic).startsWith("library/bookRequests/")) {
    Serial.print("Libro recibido con el id ");
    Serial.println(msg);
  }
}

*/

// Reconecta con MQTT broker
void reconnect() {
  // MQTT_CLIENT.setServer("192.168.1.206", 1883); // si uso un servidor local <ver IP correcta>
  MQTT_CLIENT.setServer("54.173.227.140", 1883);  // servidor gratuito

  MQTT_CLIENT.setClient(WIFI_CLIENT);

  // Intentando conectar con el broker.
  while (!MQTT_CLIENT.connected()) {
    Serial.println("Intentando conectar con MQTT.");
    MQTT_CLIENT.connect("library"); // Escribe cualquier nombre.

    // Espera antes de volver a intentarlo.
    delay(3000);
  }

  Serial.println("Conectado a MQTT.");
}
