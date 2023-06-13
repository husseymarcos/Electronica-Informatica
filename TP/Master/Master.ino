#include <Wire.h>
#include <Keypad.h>
#include <MFRC522.h>
#include <SPI.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>
#include "Card.h"
#include <SD.h>
#include <esp_now.h>
#include <WiFi.h>

// Declaracion de variables y pines
// RFID
const int RST_PIN = 22;  // Pin de reset del módulo RC522
const int SS_PIN = 5; // Pin del módulo RC522

MFRC522 rfid(SS_PIN, RST_PIN);  // Crear instancia del lector RFID

// Dirección MAC del ESP32 esclavo
uint8_t slaveAddress[] = {0xC8, 0xF0, 0x9E, 0x53, 0x05, 0xD8};

typedef struct struct_message {
  int a;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// KeyPad
const uint8_t ROWS = 4; // Número de filas
const uint8_t COLS = 4; // Número de columnas

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

uint8_t colPins[COLS] = { 33, 32, 35, 34 }; // Conexiones de los pines de las columnas
uint8_t rowPins[ROWS] = { 14, 27, 26, 25 }; // Conexiones de los pines de las filas

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char pass[7] = "123456"; // Declaracion de variable que almacena el password
char ingresaPass[7];     // Variable que almacena la clave ingresada
int indice = 0; //Variable que define el numero de letra que se va ingresando

// Leds
int pinLedVerde = 12;
int pinLedRojo = 13;
//

// Clase de Card para Testear
Card card(1234, "User 1", 4321, 123456); // code/owner/uid/password
//

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Iniciar comunicación SPI
  rfid.PCD_Init(); // Iniciar el lector RFID
  Serial.println("Parque de Diversiones");
  Serial.println("Apoye su tarjeta");
  WiFi.mode(WIFI_STA);

  // Leds
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  //

  // Codigo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t slaveInfo;
  memcpy(slaveInfo.peer_addr, slaveAddress, 6);
  slaveInfo.channel = 0;
  slaveInfo.encrypt = false;

  if (esp_now_add_peer(&slaveInfo) != ESP_OK) {
    Serial.println("Error registrando placa esclava");
    return;
  }
}

// Codigo de las operaciones
void loop() {
  // Codigo RFID reconocimiento de tarjeta.
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      Serial.print("UID: ");
      for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println(); // Imprime un salto de línea
      rfid.PICC_HaltA(); // Resetea el módulo para estar disponible para nuevas lecturas.
    }
  }
  delay(250);

  // ESPNOW
  myData.a = random(1, 20); //Definir

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(slaveAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  delay(2000);

  // Considerar aplicar una condicion para que esto aparezca.
  char key = keypad.getKey(); // Obtiene la tecla presionada y la guarda en una variable
  if (key) {
    ingresaPass[indice] = key; // Almacenamos la letra ingresada en el arreglo de letras ingresaPass
    indice++; // Hacemos como una lista que por cada index almacene el digito ingresado. Aumentamos la letra que se va a almacenar
    Serial.println("Ingrese su contraseña: ");
    Serial.print(key); // Envia a Monitor Serial la tecla presionada
    digitalWrite(pinLedVerde, LOW); // Led Verde apagado
    digitalWrite(pinLedRojo, LOW); // Led Rojo apagado
  }

  if (indice >= 6) { // Si ya se ingresaron 6 letras o mas

    if (strcmp(pass, ingresaPass) == 0) { // Si el password coincide con la clave ingresada
      Serial.println("Acceso permitido"); // Mostramos el mensaje de acceso permitido
      digitalWrite(pinLedVerde, HIGH);

      if (pinLedVerde == HIGH) {
        Serial.println("Buen día! Seleccione una opción: ");
        Serial.println();
        Serial.println("1. Transferir Créditos");
        Serial.println();
        Serial.println("2. Cargar Créditos");
        Serial.println();
        Serial.println("3. Consultar Créditos");
        Serial.println();

        if (key != NO_KEY) { // Se verifica si se presiono una tecla
          int creditToLoad = 0;
          int creditToTransfer = 0;
          int limitLoad = 1000;

        if (key) {
        switch (key) {
          case '1': {
            Serial.println("Transfer Credits");
            int amount = getAmountFromKeypad();
            if (amount > 0) {
              Serial.println("Enter recipient's user ID:");
              int recipientUserID = getUserIDFromKeypad();
              if (recipientUserID > 0) {
                Card* recipientCard = getCardByID(recipientUserID);
                if (recipientCard) {
                  card1.creditTransfer(amount, *recipientCard);
                  Serial.println("Credits transferred successfully.");
                } else {
                  Serial.println("Recipient card not found.");
                }
              } else {
                Serial.println("Invalid recipient user ID.");
              }
            } else {
              Serial.println("Invalid amount.");
            }
            break;
          }
          case '2': {
            Serial.println("Load Credits");
            int amount = getAmountFromKeypad();
            if (amount > 0) {
              card1.creditLoad(amount);
              Serial.println("Credits loaded successfully.");
            } else {
              Serial.println("Invalid amount.");
            }
            break;
          }
          case '3': {
            Serial.println("Consult Credits");
            card1.consultCredit();
            break;
          }
          default:
            Serial.println("Invalid option.");
            break;

          
      
        esp_err_t result = esp_now_send(slaveAddress, (uint8_t *) &myData, sizeof(myData));
        if (result == ESP_OK) {
          Serial.println("Mensaje enviado correctamente");
        } else {
          Serial.println("Error al enviar el mensaje.");
        }
        delay(2000);
      } else {
        Serial.println("Acceso Denegado");
        digitalWrite(pinLedRojo, HIGH);
      }

      indice = 0;
    }
  }
}
