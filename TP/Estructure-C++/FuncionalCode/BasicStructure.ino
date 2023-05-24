#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Determinamos el pin donde estan las columnas y las filas del LCD.
int lcdColumns = 20;
int lcdRows = 4;


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
uint8_t colPins[COLS] = { 16, 4, 2, 15 }; // Aca esta los pines de las columnas en el ESP.

// pines correspondientes a las columnas
//uint8_t rowPins[ROWS] = { 19, 18, 5, 17 };
uint8_t rowPins[ROWS] = { 19, 18, 5, 17 };

// crea objeto con los prametros creados previamente
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//Declaramos la variable que almacena el password
char pass[7]="123456";
//Varible que alamacena la clave que se va ingresando
char ingresaPass[7];
//Variable que define el numero de letra que se va ingresando
int indice=0;

int pinLedVerde=12;
int pinLedRojo=13;


void setup() {
  Serial.begin(115200);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Parque de Diversiones");
  delay(5000);
  lcd.clear();
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  Serial.println("Hello World!");

}



void loop() {
  //lcd.setCursor(0,0);
  //lcd.print("Entrando al loop");
  //delay(5000);
  //lcd.clear();
  //Serial.println("Entrando al loop");

  // obtiene tecla presionada y asigna a variable
  char key = keypad.getKey();
  // comprueba que se haya presionado una tecla
  if (key) {
    //Alamacenamos la letra ingresada en arreglo de letras ingresaPass
    ingresaPass[indice] = key;
    //Aumentamos la letra que se va a almacenar
    indice++; // Hacemos como una lista que por cada index almacene el digito ingresado.
    // envia a monitor serial la tecla presionada
    Serial.println(key);
    lcd.setCursor(0,0);
    lcd.print(key);
    delay(2000);
    lcd.clear();
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

      if(pinLedVerde = HIGH){ // Necesito ver como puedo pasar entre opciones
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
