#include <Wire.h>
#include <Keypad.h>
#include <MFRC522.h>
#include <SPI.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>
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

// Estructura para almacenar los datos de una tarjeta
struct Tarjeta {
  int usuario;
  int balance;
  int contrasena;
};

const int MAX_TARJETAS = 10;
Tarjeta tarjetas[MAX_TARJETAS]; // Arreglo de estructuras para almacenar las tarjetas
int numTarjetas = 0; // Número de tarjetas registradas

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

void transferirCreditos() {
  Serial.println("Transferir créditos");

  int origen, destino, cantidad;

  // Solicitar número de tarjeta de origen
  Serial.println("Ingrese el número de tarjeta de origen:");
  while (!Serial.available()) {
    // Esperar a que se ingrese un número
  }
  origen = Serial.parseInt();

  // Solicitar número de tarjeta de destino
  Serial.println("Ingrese el número de tarjeta de destino:");
  while (!Serial.available()) {
    // Esperar a que se ingrese un número
  }
  destino = Serial.parseInt();

  // Solicitar cantidad a transferir
  Serial.println("Ingrese la cantidad a transferir:");
  while (!Serial.available()) {
    // Esperar a que se ingrese un número
  }
  cantidad = Serial.parseInt();

  // Verificar que las tarjetas existan y realizar la transferencia
  bool origenEncontrado = false;
  bool destinoEncontrado = false;
  for (int i = 0; i < numTarjetas; i++) {
    if (tarjetas[i].usuario == origen) {
      origenEncontrado = true;
    }
    if (tarjetas[i].usuario == destino) {
      destinoEncontrado = true;
    }
    if (origenEncontrado && destinoEncontrado) {
      // Realizar la transferencia
      tarjetas[i].balance -= cantidad; // Restar del saldo de la tarjeta de origen
      tarjetas[i].balance += cantidad; // Sumar al saldo de la tarjeta de destino
      Serial.println("Transferencia realizada con éxito");
      return;
    }
  }

  // Si alguna de las tarjetas no fue encontrada, mostrar un mensaje de error
  Serial.println("Error: Tarjetas no encontradas");
}

void cargarCreditos() {
  Serial.println("Cargar créditos");

  int usuario, cantidad;

  // Solicitar número de tarjeta
  Serial.println("Ingrese el número de tarjeta:");
  while (!Serial.available()) {
    // Esperar a que se ingrese un número
  }
  usuario = Serial.parseInt();

  // Solicitar cantidad a cargar
  Serial.println("Ingrese la cantidad a cargar:");
  while (!Serial.available()) {
    // Esperar a que se ingrese un número
  }
  cantidad = Serial.parseInt();

  // Buscar la tarjeta y cargar los créditos
  for (int i = 0; i < numTarjetas; i++) {
    if (tarjetas[i].usuario == usuario) {
      tarjetas[i].balance += cantidad; // Sumar al saldo de la tarjeta
      Serial.println("Créditos cargados con éxito");
      return;
    }
  }

  // Si la tarjeta no fue encontrada, mostrar un mensaje de error
  Serial.println("Error: Tarjeta no encontrada");
}

void consultarCreditos() {
  Serial.println("Consultar créditos");

  int usuario;

  // Solicitar número de tarjeta
  Serial.println("Ingrese el número de tarjeta:");
  while (!Serial.available()) {
    // Esperar a que se ingrese un número
  }
  usuario = Serial.parseInt();

  // Buscar la tarjeta y mostrar el saldo
  for (int i = 0; i < numTarjetas; i++) {
    if (tarjetas[i].usuario == usuario) {
      Serial.print("Saldo de la tarjeta: ");
      Serial.println(tarjetas[i].balance);
      return;
    }
  }

  // Si la tarjeta no fue encontrada, mostrar un mensaje de error
  Serial.println("Error: Tarjeta no encontrada");
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

  // Keypad
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case '1':
        transferirCreditos();
        break;
      case '2':
        cargarCreditos();
        break;
      case '3':
        consultarCreditos();
        break;
      default:
        Serial.println("Opción inválida");
        break;
    }
  }
}
