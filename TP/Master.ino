#include <Wire.h>
#include <Keypad.h>
#include <MFRC522.h>
#include <SPI.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>
#include "Card.h"
#include <SD.h>

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

void OnSent(const uint8_t *mac_addr, esp_now_send_status_t status){
  Serial.print("\r\nStatus envio: \t");
  if(status == ESP_NOW_SEND_SUCCESS) Serial.println("Enviado Correctamente");
  else Serial.print("Envio fallido");
}
void setup() {
  Serial.begin(115200);
  SPI.begin(); // Iniciar comunicación SPI
  rfid.PCD_Init(); // Iniciar el lector RFID
  Serial.print("Parque de Diversiones");
  Serial.print("Apoye su tarjeta");

  // Leds
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  //

 // Codigo ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnSent);

  esp_now_peer_info_t slaveInfo;

  memcpy(slaveInfo.peer_addr, slaveAddress, myData.a);
  slaveInfo.channel = 0;
  slaveInfo.encrypt = false;

  if(esp_now_add_peer(&slaveInfo) != ESP_OK){
    Serial.println("Error registrando placa esclava");
    return;
  }



}
//

// Codigo de las operaciones

void loop() {
  /*
  * PICC_IsNewCardPresent(): Indica si se ha detectado un tag cerca del lector.
  * PICC_ReadCardSerial(): Lee la información dentro del tag.
  */
  // Codigo RFID reconocimiento de tarjeta.
  if(rfid.PICC_IsNewCardPresent()){
    if(rfid.PICC_ReadCardSerial()){
      Serial.print("UID: ");
      for(byte i = 0; i < rfid.uid.size; i++){
        Serial.println(rfid.uid.uidByte[i] < 0x10 ? " 0": " ");
        Serial.println(rfid.uid.uidByte[i], HEX);
      }
    Serial.println(); // Imprime un salto de línea
    rfid.PICC_HaltA(); // Resetea el módulo para estar disponible para nuevas lecturas.

    }
  }
  delay(250);

  // Considerar aplicar una condicion para que esto aparezca.
  char key = keypad.getKey();       // Obtiene la tecla presionada y la guarda en una variable
  if (key){
    ingresaPass[indice]=key;        // Almacenamos la letra ingresada en el arreglo de letras ingresaPass
    indice++;                       // Hacemos como una lista que por cada index almacene el digito ingresado. Aumentamos la letra que se va a almacenar
    Serial.println("Ingrese su contraseña: ");
    Serial.print(key);         // Envia a Monitor Serial la tecla presionada
    digitalWrite(pinLedVerde, LOW); // Led Verde apagado
    digitalWrite(pinLedRojo, LOW);  // Led Rojo apagado
  }

  if(indice >= 6){                        // Si ya se ingresaron 6 letras o mas

    if(strcmp(pass,ingresaPass)==0){      // Si el password coincide con la clave ingresada

      Serial.println("Acceso permitido"); // Mostramos el mensaje de acceso permitido
      digitalWrite(pinLedVerde, HIGH);

      if(pinLedVerde = HIGH){
        Serial.println("Buen día! Seleccione una opción: ");
        Serial.println();
        Serial.println("1. Transferir Créditos");
        Serial.println();
        Serial.println("2. Cargar Créditos");
        Serial.println();
        Serial.println("3. Consultar Créditos");
        Serial.println();

        if(key != NO_KEY){ // Se verifica si se presiono una tecla
          int creditToLoad = 0;
          int creditToTransfer = 0;
          int limitLoad = 1000;

          switch(key){

            case '1': // Transferir Creditos
                Serial.println("Seleccione al usuario al que quiere transferir");
                Serial.println("CANCELAR (*)");
                Serial.println("ACEPTAR(#)");

                while(creditToTransfer >= 0 && creditToTransfer <= limitLoad){ // Fragmento de codigo que sirve para cuanto monto le queres transmitir

                  if(key >= '0' && key <= '9'){
                    creditToTransfer = creditToTransfer*10 + int(key);
                    Serial.print(key);
                  }

                  else if(key != NO_KEY){

                      if(key == '*') Serial.print("Operacion Cancelada");

                      else if(key == '#') card.CreditTransfer(creditToTransfer, card.findUserNameByID(key), card.owner, card.findUserNameByID(key));

                      else Serial.print("Opción no valida");
                  }

                  else Serial.print("Te pasaste de la carga de créditos, ingresa menos");

                }
              break;

            case '2': // Cargar Creditos
                Serial.println("Indique cuánto monto desea cargar: ");
                if(key >= '0' && key <= '9'){
                    creditToLoad = creditToLoad*10 + (key - '0');
                    Serial.print(key);
                }

                else if(key != NO_KEY){
                    if(key == '*'){ // Cancela la operacion
                      Serial.print("Operacion Cancelada");
                      }

                    else if(key == '#') card.CreditLoad(creditToLoad, card.owner); // Realiza la operacion

                    else Serial.print("Opción no valida");
                }
              break;

            case '3': // Consultar Creditos
              card.ConsultCredit();
            break;

            default:
              Serial.print("Opción no válida.");
            break;
          }
        }
        esp_err_t result = esp_now_send(slaveAddress, (uint8_t *) &myData, sizeof(myData));
        if(result == ESP_OK){
          Serial.println("Mensaje enviado correctamente");
          }
        else{
          Serial.println("Error al enviar el mensaje.");
        }
        delay(2000);
      }
      else{
        Serial.println("Acceso Denegado");
        digitalWrite(pinLedRojo, HIGH);
      }

      indice = 0;
    }
  }
}
