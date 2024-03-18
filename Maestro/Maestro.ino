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

// Estructura para almacenar los datos de una tarjeta
struct Tarjeta {
  int usuario;
  int balance;
  int contra;
};

Tarjeta usuarios[100];
int numUsuarios = 0;



void setup() {
  Serial.begin(115200);
  SPI.begin(); // Iniciar comunicación SPI
  rfid.PCD_Init(); // Iniciar el lector RFID
  Serial.println();
  Serial.println();
  Serial.println("Bienvenido al Parque de Diversiones!");
  Serial.println("Apoye su tarjeta");
  WiFi.mode(WIFI_STA);

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
  String contraStr;
  int creditosTransferir = 0;
  Serial.println("Ha seleccionado (1) Transferir Creditos");
  Serial.println();
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

  Serial.println("Ingrese contraseña:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else {
        contraStr += key;
        Serial.print(key);
      }
    }
  }

  int contra = contraStr.toInt();
  if (contra == usuarios[usuarioOrigen].contra) {

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

    Tarjeta& tarjetaDestino = usuarios[usuarioDestino];
    Tarjeta& tarjetaOrigen = usuarios[usuarioOrigen]; // Obtener referencia a la tarjeta de destino

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
      Serial.println();

      myData.operation = 1;
      myData.usuarioOrigen = usuarioOrigen;
      myData.usuarioDestino = usuarioDestino;
      myData.creditosTransferir = creditosTransferir;
    } else {
      Serial.println("Saldo insuficiente en el usuario de origen");
    }
  } else {
    Serial.println("Contraseña incorrecta");
  }
}

void cargarCreditos() {
  String usuarioStr;
  int creditosTransferir = 0;
  String contraStr;
  Serial.println("Ha seleccionado (2) Cargar Creditos");
  Serial.println();
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

  Serial.println("Ingrese contraseña:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else {
        contraStr += key;
        Serial.print(key);
      }
    }
  }

  int contra = contraStr.toInt();
  if (contra = usuarios[usuario].contra){
    Serial.println("Ingrese la cantidad de créditos a cargar:");
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
}


void consultarCreditos() {
  String usuarioOrigenStr;
  String contraStr;
  Serial.println("Ha seleccionado (5) Consultar Creditos");
  Serial.println();
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

  Serial.println("Ingrese contraseña:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else {
        contraStr += key;
        Serial.print(key);
      }
    }
  }

  int contra = contraStr.toInt();
  if (contra = usuarios[usuarioOrigen].contra){
    // Mostrar información del saldo
    Serial.print("Usuario: ");
    Serial.println(usuarios[usuarioOrigen].usuario);
    Serial.print("Saldo actual: ");
    Serial.println(usuarios[usuarioOrigen].balance);

  myData.operation = 3;
  myData.usuarioOrigen = usuarioOrigen;
  myData.usuarioDestino = 0;
  myData.creditosTransferir = 0;

  esp_err_t result = esp_now_send(slaveAddress, (uint8_t *)&myData, sizeof(myData));

  }
}

void registrarUsuario() {
  String usuarioStr;
  String balanceStr;
  String contraStr;
  Serial.println("Ha seleccionado (4) Registrar Usuario");
  Serial.println();

  Serial.println("Ingrese un numero para identificarse:");
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

  int usuario = usuarioStr.toInt();

  Serial.println();

  for (int i = 0; i < numUsuarios; i++) {
    if (usuarios[usuario].usuario == usuario) {
      Serial.println("\n¡Error! El número de usuario ya ha sido registrado.");
      return;
    }
  }

  Serial.println("Ingrese contraseña deseada:");
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        Serial.println(); // Salto de línea
        break;
      } else {
        contraStr += key;
        Serial.print(key);
      }
    }
  }

  int contra = contraStr.toInt();

  Serial.println();

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
 
  usuarios[usuario].usuario = usuario;
  usuarios[usuario].balance = balance;
  usuarios[usuario].contra = contra;

  Serial.println("Usuario creado con exito!");

  numUsuarios++;

}

void showMenu() {
  Serial.println("Seleccione una opción:");
  Serial.println("1. Transferir créditos");
  Serial.println("2. Cargar créditos");
  Serial.println("3. Consultar créditos");
  Serial.println("4. Crear usuario");
}

















// Codigo de las operaciones
void loop(){
  static bool menuDisplayed = false;
  
  // Codigo RFID reconocimiento de tarjeta.
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      Serial.println("Seleccione una opción:");
      Serial.println("1. Transferir créditos");
      Serial.println("2. Cargar créditos");
      Serial.println("5. Consultar créditos");
      Serial.println("4. Crear usuario");
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
        Serial.println();
        Serial.println("Seleccione una opción:");
        Serial.println("1. Transferir créditos");
        Serial.println("2. Cargar créditos");
        Serial.println("5. Consultar créditos");
        Serial.println("4. Crear usuario");
        Serial.println();
        break;
      case '2':
        cargarCreditos();
        Serial.println();
        Serial.println("Seleccione una opción:");
        Serial.println("1. Transferir créditos");
        Serial.println("2. Cargar créditos");
        Serial.println("5. Consultar créditos");
        Serial.println("4. Crear usuario");
        Serial.println();
        break;
      case '5':
        consultarCreditos();
        Serial.println();
        Serial.println("Seleccione una opción:");
        Serial.println("1. Transferir créditos");
        Serial.println("2. Cargar créditos");
        Serial.println("5. Consultar créditos");
        Serial.println("4. Crear usuario");
        Serial.println();
        break;
      case '4':
        registrarUsuario();
        Serial.println();
        Serial.println("Seleccione una opción:");
        Serial.println("1. Transferir créditos");
        Serial.println("2. Cargar créditos");
        Serial.println("5. Consultar créditos");
        Serial.println("4. Crear usuario");
        Serial.println();
        break;
      default:
        Serial.println("Opción inválida");
        break;
    }
  }
}
