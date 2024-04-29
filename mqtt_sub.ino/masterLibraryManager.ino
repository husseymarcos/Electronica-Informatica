#include <Wire.h>
#include <Keypad.h>
#include <SD.h>
#include <map>
#include <MFRC522.h>


/* clases que tendríamos que revaluar:
- Adafruit_NeoPixel
- SPI

En relación al resto, se usaron para el esp now
*/

// Declaracion de variables y pines para el RFID

const int RST_PIN = 22; // Pin de Reset del módulo RC522
const int SS_PIN = 5; // Pin del módulo RC522

MFRC522 rfid(SS_PIN, RST_PIN); // Crear instancia del lector RFID


// Keypad
const uint8_t ROWS = 4;
const uint8_t COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

uint8_t colPins[COLS] = { 33, 32, 35, 34 }; // Conexiones de los pines de las columnas
uint8_t rowPins[ROWS] = { 14, 27, 26, 25 }; // Conexiones de los pines de las filas

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


// Estructura para almacenar datos de una tarjeta

struct Tarjeta {
  int usuario;
  int puntosActuales;
  int contra;
};

Tarjeta usuarios[100];

int numUsuarios = 0;

/* 
-----------------------------------------------------INFORMACIÓN QUE DEBERÍA EVALUARSE--------------------------------------------------------

Una cosa a evaluar, es cómo hacemos desde la página web, otorgar los puntos de lectura que debería recibir el usuario.

Otra cosa a evaluar, es cómo vinculamos el registro de usuario a la base de datos de la página web. 

Ver como Mosquitto envía el libro que quiere el usuario, además de otorgarles los PL relacionados a éste. 

*/


void setup(){
  Serial.begin(115200);
  //SPI.begin(); -> No sé para que sirve
  rfid.PCD_Init();
  Serial.println();
  Serial.println();
  Serial.println("Bienvenido a LibrosExpress!");
  Serial.println("Apoye su tarjeta");

}


void showMenu(){
  Serial.println("Seleccione una opción: ");
  Serial.println("1. Registrar Usuario");
  Serial.println("2. Consultar PL");
}

void registrarUsuario(){
  String usuarioStr;
  String contraStr;

  Serial.println("Ha seleccionado Registrar Usuario"); // Estaría bueno ver una seccion de usuarios también en la página web
  Serial.println();

  Serial.println("Ingrese un número para identificarse: ");

  while(true){
    char key = keypad.getKey();
    if(key){
      if(key == '*'){
        Serial.println();
        break;
      } else if(key >= '0' && key <= '9'){
        usuarioStr += key;
        Serial.print(key);

      }
    }
  }

  int usuario = usuarioStr.toInt();

  Serial.println();

  for(int i = 0; i < numUsuarios; i++){
    if(usuarios[usuario].usuario == usuario){
      Serial.println("\n Error! El número de usuario ya ha sido registrado.");
    }
  }

  Serial.println("Ingrese contraseña deseada: ");

  while(true){
    char key = keypad.getKey();

    if(key){
      if(key == '*'){
        Serial.println();
        break;
      } else{
        contraStr += key;
        Serial.print(key);
      }
    }
  }

  int contra = contraStr.toInt();

  Serial.println();

  Serial.println("Usuario creado con exito!");


  numUsuarios++;
}

void consultarPL(){
  String usuarioOrigenStr;
  String contraStr;
  Serial.println("Ha seleccionado Consultar PL");
  Serial.println();
  Serial.println("Ingrese el numero de usuario: ");

  while(true){
    char key = keypad.getKey();

    if(key){
    if(key == '*'){
      Serial.println();
      break;

     } else{
      usuarioOrigenStr += key;
      Serial.print(key);
     }
    }
  }

  int usuarioOrigen = usuarioOrigenStr.toInt();

  Serial.println("Ingrese contraseña: ");

  while(true){
    char key = keypad.getKey();
    if(key){
      if(key == '*'){
        Serial.println();
        break;
      } else{
        contraStr += key;
        Serial.print(key);
      }
    }
  }

  int contra = contraStr.toInt();

  if(contra = usuarios[usuarioOrigen].contra){
    // Mostrar información de los puntos de lectura
    Serial.print("Usuario: ");
    Serial.println(usuarios[usuarioOrigen].usuario);
    Serial.print("Saldo Actual: ");
    Serial.println(usuarios[usuarioOrigen].puntosActuales); 
  }

}


// ......................................................................

// Codigo de las operaciones

void loop(){
  static bool menuDisplayed = false;

  // Codigo RFID reconocimiento de la tarjeta.
  if(rfid.PICC_IsNewCardPresent()){
    if(rfid.PICC_ReadCardSerial()){
      Serial.println("Selecciones una opción: ");
      Serial.println("1. Registrar Usuario");
      Serial.println("2. Consultar PL");
      Serial.println(); 
      rfid.PICC_HaltA(); // Resetea el módulo para estar disponible para nuevas lecturas
    }
  }

  delay(250);


  // Keypad
  char key = keypad.getKey();

  if(key){
    switch(key){
      case '1':
        registrarUsuario();
        Serial.println();
        Serial.println("Selecciones una opción: ");
        Serial.println("1. Registrar Usuario");
        Serial.println("2. Consultar PL");
        Serial.println();
        break;

      case '2':
        consultarPL();
        Serial.println();
        Serial.println("Selecciones una opción: ");
        Serial.println("1. Registrar Usuario");
        Serial.println("2. Consultar PL");
        Serial.println();
        break;

      default:
        Serial.println("Opción Inválida");
        break;
    }
  }
}