

#include <Keypad.h>           // Se incluye la librería Keypad.
#include <MFRC522.h>          // Se incluye la librería del módulo RFID
#include <SPI.h>              // Se incluye la librería del SPI
#include <LiquidCrystal.h>    // Se incluye la librería del LCD
#include "Card.h"             // Se incluye la clase de las operaciones de las tarjetas

const char* fileName = "usuarios.txt"; // Sirve para el recorrido que realice de los archivos de la micro sd en algunos metodos mas abajo

int lcdColumns = 20; // Sabemos que nuestro lcd es de 20x4

int lcdRows = 4;

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


char pass[7] = "123456"; // Declaracion de variable que almacena el password

char ingresaPass[7];     // Variable que almacena la clave ingresada

int indice = 0; //Variable que define el numero de letra que se va ingresando

int pinLedVerde=12; // Defino pin determinado para el Led Verde. Parte Positiva del Led
int pinLedRojo=13;  // Defino pin determinado para el Led Rojo. Parte Positiva del Led


int botonVolver = 7;

int botonSiguiente = 6;


Card card = Card(1234, "User 1"); // Sirve para realizar algunos metodos que realice mas abajo.

int paginaActual = 1; // En la pagina donde estas parado, sería que vemos del lcd.

int cantidaddePaginas = 3; // Como cantidad de "paginas" que verias en el lcd.

//
//
//

void setup() {
  // put your setup code here, to run once:
  lcd.init();      // Inicializamos el LCD
  lcd.backlight(); // Enciende la luz del LCD
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  pinMode
  Serial.begin(115200);
  pinMode(botonVolver, INPUT_PULLUP);
  pinMode(botonSiguiente, INPUT_PULLUP);

}

//
//
//

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
        delay(5000);
        lcd.setCursor(0, 3);
        lcd.print("Volver <<"); // Va a mostrarse siempre en el lcd, permitiria retroceder en la "pagina"

        if(paginaActual == 1){ // Si estamos en la primer pagina, no puede volver ->
          lcd.setCursor(0, 3);
          lcd.print(); // -> Por tanto hago que no aparezca esa opcion
        }
        lcd.setCursor(14, 3);
        lcd.print("Siguiente >>"); // Va a mostrarse siempre en el lcd, permitiria avanzar en la "pagina"

        if(paginaActual == cantidaddePaginas){ // Si estamos en la ultima pagina, no puede haber siguiente ->
          lcd.setCursor(14, 3);
          lcd.print(); // -> Por tanto hago que no aparezca esa opcion
        }

        switch(paginaActual){
          case 1:
            lcd.setCursor(0,0);
            lcd.print("1. Transferir Creditos");
            break;
          case 2:
            lcd.setCursor(0, 0);
            lcd.print("2. Cargar Creditos");
            break;
          case 3:
            lcd.setCursor(0, 0);
            lcd.print("3. Consultar Creditos")
        }

        if(digitalRead(botonVolver) == LOW){
          delay(50);
          paginaActual = paginaActual - 1;

          if(paginaActual == 0){
            paginaActual = cantidaddePaginas;
          }
        }

        if(digitalRead(botonSiguiente) == LOW){
          delay(50);
          paginaActual = paginaActual + 1;

          if(paginaActual == cantidaddePaginas + 1){
            paginaActual = 1;
          }
        }

        delay(10);


//
//
//

        if(key != NO_KEY){ // Se verifica si se presiono una tecla
          switch(key){
            case '1': // Transferir Creditos
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.println("Seleccione el usuario al que quiere transferir: ");
                File file = SD.open(fileName, FILE_READ);

                if(file){ // Con este codigo muestro segun como presione los botones a los distintos usuarios.
                  while(file.available()){
                    String line = file.readStringUntil('\n');
                    if(digitalRead(botonSiguiente) == HIGH && paginaActual != cantidaddePaginas + 1){ // Mostrame al siguiente y que no se pase de paginas.
                      lcd.setCursor(0, 1);
                      lcd.print(line);
                    }

                    if(digitalRead(botonVolver) == HIGH && paginaActual != 0){ // mostrame al anterior y que no se pase de paginas.
                      lcd.setCursor(0, 1);
                      lcd.print(line);
                    }
                  }
                  file.close();
                }
                else{
                  lcd.setCursor(0, 1);
                  lcd.print("Error al abrir archivo.");
                }

                int creditTotransfer = 0;
                while(creditToTransfer >= 0){ // Fragmento de codigo que sirve para cuanto monto le queres transmitir
                  if(key >= '0' && key <= '9'){
                    creditToTransfer = creditToTransfer*10 + int(key);
                    lcd.print(key);
                  }
                  else if(digitalRead(botonAnterior) == HIGH){
                        break;
                  }
                  else if(digitalRead(botonSiguiente) == HIGH){
                    CreditTransfer(creditToTransfer, findUser(key), card.owner, findUser(key));
                  }

                }
              break;

            case '2': // Cargar Creditos
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.println("Indique cuánto monto desea cargar: ");

                int creditToLoad = 0;
                while(true){
                  if(key >= '0' && key <= '9'){
                    creditToLoad = creditToLoad*10 + (key - '0');
                    lcd.print(key);
                  }
                  else if(digitalRead(botonVolver) == HIGH){break;} // Cancela la operacion.

                  else if(digitalRead(botonSiguiente) == HIGH){ // Realiza la operacion
                      CreditLoad(creditToLoad, card.owner); // Tiene como argumentos el saldo que debería cargarse y el usuario Owner de la tarjeta.
                  }
                }
              break;


            case '3': // Consultar Creditos
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(ConsultCredit());
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