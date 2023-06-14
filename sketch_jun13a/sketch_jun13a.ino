#include <Wire.h>
#include <Keypad.h>
#include <MFRC522.h>
#include <SPI.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>
#include <SD.h>
#include <esp_now.h>
#include <WiFi.h>
#include <map>



// Declaracion de variables y pines
// RFID
const int RST_PIN = 22;  // Pin de reset del módulo RC522
const int SS_PIN = 5; // Pin del módulo RC522

MFRC522 rfid(SS_PIN, RST_PIN);  // Crear instancia del lector RFID

// Dirección MAC del ESP32 esclavo
uint8_t slaveAddress[] = {0xC8, 0xF0, 0x9E, 0x53, 0x05, 0xD8};

typedef struct struct_message {
  int operation;
  int usuarioOrigen;
  int usuarioDestino;
  int creditosTransferir;
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


Tarjeta usuarios[100];
int numUsuarios = 0;

const int MAX_TARJETAS = 10;
Tarjeta tarjetas[MAX_TARJETAS]; // Arreglo de estructuras para almacenar las tarjetas
int numTarjetas = 0;












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
  String usuarioOrigenStr;
  String usuarioDestinoStr;
  int creditosTransferir = 0;

  Serial.println("Ingrese el número de usuario origen:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else {
        usuarioOrigenStr += key;
        Serial.print(key);
      }
    }
  }

  int usuarioOrigen = usuarioOrigenStr.toInt();

  Serial.println("Ingrese el número de usuario destino:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else {
        usuarioDestinoStr += key;
        Serial.print(key);
      }
    }
  }

  int usuarioDestino = usuarioDestinoStr.toInt();

  Serial.println("\nIngrese la cantidad de créditos a transferir:");
  while (true) {
    char key = keypad.getKey();
    if (key == '#') {
      Serial.println("Transferencia cancelada");
      return;
    } else if (key) {
      if (key >= '0' && key <= '9') {
        creditosTransferir = creditosTransferir * 10 + (key - '0');
        Serial.print(key);
      } else if (key == '*') {
        break;
      }
    }
  }
  
 // Verificar si el usuario de origen existe en el mapa de tarjetas
  if (numUsuarios > 0) {
    Tarjeta& tarjetaOrigen = usuarios[usuarioOrigen];  // Obtener referencia a la tarjeta de origen

    // Verificar si el usuario de destino existe en el mapa de tarjetas
    if (numUsuarios > 0) {
      Tarjeta& tarjetaDestino = usuarios[usuarioDestino];  // Obtener referencia a la tarjeta de destino

      // Verificar si el usuario de origen tiene suficiente saldo para transferir
      if (tarjetaOrigen.balance >= creditosTransferir) {
        // Realizar la transferencia de créditos
        tarjetaOrigen.balance -= creditosTransferir;
        tarjetaDestino.balance += creditosTransferir;

        // Mostrar información de la transferencia
        Serial.println("Transferencia de créditos realizada:");
        Serial.print("Usuario origen: ");
        Serial.println(usuarioOrigen);
        Serial.print("Usuario destino: ");
        Serial.println(usuarioDestino);
        Serial.print("Cantidad transferida: ");
        Serial.println(creditosTransferir);
      } else {
        Serial.println("Saldo insuficiente en el usuario de origen");
      }
    } else {
      Serial.println("Usuario destino no encontrado");
    }
  } else {
    Serial.println("Usuario de origen no encontrado");
  }

  myData.operation = 1;
  myData.usuarioOrigen = usuarioOrigen;
  myData.usuarioDestino = usuarioDestino;
  myData.creditosTransferir = creditosTransferir;
}

void cargarCreditos() {
  String usuarioStr;
  int creditosTransferir = 0;

  Serial.println("Ingrese el número de usuario:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else {
        usuarioStr += key;
        Serial.print(key);
      }
    }
  }
  int usuario = usuarioStr.toInt();

  Serial.println("\nIngrese la cantidad de créditos a cargar:");
  while (true) {
    char key = keypad.getKey();
    if (key == '#') {
      Serial.println("Transferencia cancelada");
      return;
    } else if (key) {
      if (key >= '0' && key <= '9') {
        creditosTransferir = creditosTransferir * 10 + (key - '0');
        Serial.print(key);
      } else if (key == '*') {
        break;
      }
    }
  }

  if (numUsuarios > 0) {
    Tarjeta& tarjeta = usuarios[usuario];  // Obtener referencia a la tarjeta de origen

    // Verificar si el usuario de destino existe en el mapa de tarjetas
    if (numUsuarios > 0) {
        tarjeta.balance += creditosTransferir;

        // Mostrar información de la transferencia
        Serial.println("Carga de créditos realizada:");
        Serial.print("Usuario: ");
        Serial.println(usuario);
        Serial.print("Cantidad cargada: ");
        Serial.println(creditosTransferir);
    }
  }
  myData.operation = 2;
  myData.usuarioOrigen = usuario;
  myData.usuarioDestino = 0;
  myData.creditosTransferir = creditosTransferir;
}


void consultarCreditos() {
  String usuarioOrigenStr;

  Serial.println("Ingrese el número de usuario:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else {
        usuarioOrigenStr += key;
        Serial.print(key);
      }
    }
  }
  int usuarioOrigen = usuarioOrigenStr.toInt();

    // Mostrar información del saldo
  Serial.print("Usuario: ");
  Serial.println(usuarios[usuarioOrigen].usuario);
  Serial.print("Saldo actual: ");
  Serial.println(usuarios[usuarioOrigen].balance);

  myData.operation = 3;
  myData.usuarioOrigen = usuarioOrigen;
  myData.usuarioDestino = 0;
  myData.creditosTransferir = 0;
}

void registrarUsuario() {
  String usuarioStr;
  String balanceStr;


  Serial.println("Ingrese el número de usuario:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else if (key >= '0' && key <= '9') {
        usuarioStr += key;
        Serial.print(key);
      }
    }
  }

  Serial.println(); // Salto de línea

  Serial.println("Ingrese el balance del usuario:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else if (key >= '0' && key <= '9') {
        balanceStr += key;
        Serial.print(key);
      }
    }
  }
  Serial.println(); // Salto de línea

  int balance = balanceStr.toInt();
  int usuario = usuarioStr.toInt();

  // Guardar el usuario en el arreglo de usuarios

  

  usuarios[usuario].usuario = usuario;
  usuarios[usuario].balance = balance;

  numUsuarios++;

}

void showMenu() {
  Serial.println("Seleccione una opción:");
  Serial.println("1. Transferir créditos");
  Serial.println("2. Cargar créditos");
  Serial.println("3. Consultar créditos");
}

















// Codigo de las operaciones
void loop(){
  static bool menuDisplayed = false;
  
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

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(slaveAddress, (uint8_t *) &myData, sizeof(myData));


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
      case '5':
        consultarCreditos();
        break;
      case '4':
        registrarUsuario();
        break;
      default:
        Serial.println("Opción inválida");
        break;
    }
  }
}
