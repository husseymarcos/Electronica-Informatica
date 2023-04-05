/*Lo que permite el SCAN es saber la posición hexagesimal del display que voy a utilizar. De esta forma si quiero, 
por ejemplo si quiero mostrar en un display "hello world" lo que haría con el SCAN es determinar la posición hexagesimal
que tiene ese display en cuestión. 

Lo explicamos con el nro de telefono. Si cada display tiene un nro de telefono distinto, el SCAN permite conocer el nro de telefono
de cada display. */

// importamos la libreria Keypad
#include <Keypad.h>

// define numero de filas
const uint8_t ROWS = 4;

// define numero de columnas
const uint8_t COLS = 4;

// define la distribucion de teclas
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// pines correspondientes a las filas
uint8_t colPins[COLS] = { 16, 4, 2, 15 };

// pines correspondientes a las columnas
uint8_t rowPins[ROWS] = { 19, 18, 5, 17 };

// crea objeto con los prametros creados previamente
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//Declaramos la variable que almacena el password
char pass[7]="ABCD45";
//Varible que alamacena la clave que se va ingresando
char ingresaPass[7];
//Variable que define el numero de letra que se va ingresando
int indice=0;

int pinLedVerde=12;
int pinLedRojo=13;

void setup() {
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // obtiene tecla presionada y asigna a variable
  char key = keypad.getKey();
  // comprueba que se haya presionado una tecla
  if (key) {
    //Alamacenamos la letra ingresada en arreglo de letras ingresaPass
    ingresaPass[indice]=key;
    //Aumentamos la letra que se va a almacenar
    indice++; // Hacemos como una lista que por cada index almacene el digito ingresado.
    // envia a monitor serial la tecla presionada
    Serial.println(key);
    digitalWrite(pinLedVerde, LOW);
    digitalWrite(pinLedRojo, LOW);
  }

  //Si ya se ingresaron 6 letras o mas
  if(indice>=6){
    //Si el password coincide con la clave ingresada
    if(strcmp(pass,ingresaPass)==0){
      // Mostramos el mensaje de acceso permitido
      Serial.println("Acceso permitido");
      digitalWrite(pinLedVerde, HIGH);
    }
    //Si el password NO coincide con la clave ingresada
    else{
      // Mostramos el mensaje de acceso denegado
      Serial.println("Acceso denegado");
      digitalWrite(pinLedRojo, HIGH);
    }
    //Regresamos el indice a 0 para ingresar nuevas letras al password
    indice=0;
  }
}
