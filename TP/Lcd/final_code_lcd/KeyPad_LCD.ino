

/*Lo que permite el SCAN es saber la posición hexagesimal del display que voy a utilizar. De esta forma si quiero, 
por ejemplo si quiero mostrar en un display "hello world" lo que haría con el SCAN es determinar la posición hexagesimal
que tiene ese display en cuestión. 

Lo explicamos con el nro de telefono. Si cada display tiene un nro de telefono distinto, el SCAN permite conocer el nro de telefono
de cada display. */

// importamos la libreria Keypad, y la librería LiquidCrystal_I2C

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Determinamos el pin donde estan las columnas y las filas del LCD.
int lcdColumns = 16;
int lcdRows = 2;


// establecezco dirección LCD, número de columnas y filas
// si no conoce su dirección de visualización, ejecute un boceto de escáner I2C. Eso lo hace el método SCAN.

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

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
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
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

      if(pinLedVerde = HIGH){
        lcd.setCursor(0, 0);
        lcd.print("Buen dia! Seleccione una opcion:"); // Para poder ver otras opciones pensaba en que podemos usar botones.
        delay(10000);
        lcd.clear();
        option1 = lcd.print("Opcion 1: Transferir Creditos");
        delay(10000);
        lcd.clear();
        option2 = lcd.print("Opcion 2: Cargar Creditos");
        delay(10000);
        lcd.clear();
        option3 = lcd.print("Opcion 3: Consultar Creditos");
        delay(10000);
        lcd.clear();
        option4 = lcd.print("Opcion 4: Jugar un Juego");
        delay(10000);
        lcd.clear();

        // Como trato con los botones?
        if(option1 == keys[1][1]){
                // Realizar la operacion de Transferir
        }
        if(option2 == keys[1][2]){
            // Realizar la operacion de cargar creditos
        }
      }
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
