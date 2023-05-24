// Realizo el análisis del código en base a los métodos de Lucas ya hechos.
// Me encargo ahora de desarrollar, el tema de los botones, según las opciones que presiono

// Necesitamos importar el Keypad, el SPI y las tarjetas.

#include <Keypad.h>           // Se incluye la librería Keypad.
#include <MFRC522.h>          // Se incluye la librería del módulo RFID
#include <SPI.h>              // Se incluye la librería del SPI
#include <LiquidCrystal.h>    // Se incluye la librería del LCD
#include "Card.h"             // Se incluye la clase de las operaciones de las tarjetas

int lcdColumns = 16;

int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

const uint8_t ROWS = 4; // Defino nro filas
const uint8_t COLS = 4; // Defino nro columnas

// Defino Distribucion de Teclas
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

uint8_t rowPins[ROWS] = { 16, 4, 2, 15 }; // Defino pines de filas

uint8_t colPins[COLS] = { 19, 18, 5, 17 }; // Defino pines de columnas


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Creo objeto con los prametros creados previamente


char pass[7] = "ABCD45"; // Declaracion de variable que almacena el password

char ingresaPass[7];     // Variable que almacena la clave ingresada

int indice = 0; //Variable que define el numero de letra que se va ingresando

int pinLedVerde=12; // Defino pin determinado para el Led Verde. Parte Positiva del Led
int pinLedRojo=13;  // Defino pin determinado para el Led Rojo. Parte Positiva del Led

// int creditsCard1 = 10;
// int creditsCard2 = 10;

void setup() {
  // put your setup code here, to run once:
  lcd.init();      // Inicializamos el LCD
  lcd.backlight(); // Enciende la luz del LCD
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();       // Obtiene la tecla presionada y la guarda en una variable

   if (key) {
    ingresaPass[indice]=key;        // Almacenamos la letra ingresada en el arreglo de letras ingresaPass
    indice++;                       // Hacemos como una lista que por cada index almacene el digito ingresado. Aumentamos la letra que se va a almacenar
    Serial.println(key);            // Envia a Monitor Serial la tecla presionada
    digitalWrite(pinLedVerde, LOW); // Led Verde apagado
    digitalWrite(pinLedRojo, LOW);  // Led Rojo apagado
  }


  if(indice >= 6){ // Si ya se ingresaron 6 letras o mas

    if(strcmp(pass,ingresaPass)==0){      // Si el password coincide con la clave ingresada

      Serial.println("Acceso permitido"); // Mostramos el mensaje de acceso permitido
      digitalWrite(pinLedVerde, HIGH);

      if(pinLedVerde = HIGH){
        lcd.setCursor(0, 0);
        lcd.print("Buen dia! Seleccione una opcion:"); // Para poder ver otras opciones pensaba en que podemos usar botones.
        delay(10000);
        lcd.clear();
        lcd.print("Opcion 1: Transferir Creditos");
        delay(10000);
        lcd.clear();
        lcd.print("Opcion 2: Cargar Creditos");
        delay(10000);
        lcd.clear();
        lcd.print("Opcion 3: Consultar Creditos");
        delay(10000);
        lcd.clear();

        if(key != NO_KEY){        // Se verifica si se presiono una tela
          switch(key){
            case '1':
                CreditTransfer(); // Tiene como argumentos, el otro jugador y el monto que quiero pasarle
              break;
            case '2':
                CreditLoad();      // Tiene como argumentos el saldo que debería cargarse.
              break;
            case '3':
                ConsultCredit();     // Retorna el saldo actual.
              break;
            default:
              Serial.println("Opción no válida.");
              break;
          }
        }

      }
    }

    else{ // Si el password NO coincide con la clave ingresada
        Serial.println("Acceso denegado"); // Muestro por Monitor Serial el mensaje de acceso denegado. Aviso de "La clave no coincide".
        digitalWrite(pinLedRojo, HIGH);
    }

    indice=0; //Regresamos el indice a 0 para ingresar nuevas letras al password. Esto para reintentar

  }

}