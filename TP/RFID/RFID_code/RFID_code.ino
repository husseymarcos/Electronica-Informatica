#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal.h>

// '#define' crea variables SIN especificar tipo.
#define SS_PIN 10
#define RST_PIN 9

#define LED_DENIED_PIN 7
#define LED_ACCESS_PIN 6

// Defino instancias de las clases de los modulos. LiquidCrystal corresponde al uso del LCD, y MFRC522 al lector RFID. 
LiquidCrystal_I2C lcd(0x3F, 16, 2);
MFRC522 mrfc522(SS_PIN, RST_PIN);
 
// El UID contenido las tarjetas se compara con este codigo. Si coincide, se permite el acceso. 
int code[] = {32, 154, 149, 117};
int codeRead = 0;
String uidString;

void setup() {
  Serial.begin(9600);
  SPI.begin();        // Inicializo SPI.
  mfrc522.PCD_Init(); // Inicializo MFRC522
  Serial.println("Arduino RFID leyendo UID");

  pinMode(LED_DENIED_PIN, OUTPUT);
  pinMode(LED_ACCESS_PIN, OUTPUT);

  lcd.init(); // Inicializo LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Apoye su tarjeta.");

}

void loop() {
  
  if( mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()){

      lcd.clear();
      Serial.print("Tag UID:");
      lcd.setCursor(0, 0);
      lcd.print("Tag UID:");
      lcd.setCursor(0, 1);

      // La variable 'i' es un byte.
      for (byte i = 0; i < mfrc522.uid.size; i++ ) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        // Imprimo en el LCD el codigo pasado a HEXA.
        lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        lcd.print(mfrc522.uid.uidByte[i], HEX);
      }

      Serial.println();    

      int i = 0;
      boolean match = true;
      while( i < mfrc522.uid.size) {

        // Se verifica si alguno de los bytes NO coincide con el codigo. Si no, continua la ejecucion y se incrementa 'i'.
        if ( !( int( mfrc522.uid.uidByte[i]) == int(code[i]))){
          match = false;
        }
        i++;
      }

      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);

      // Si 'match' se mantiene True luego del while, se enciende 'LED_ACCESS_PIN'.
      if (match) {
        digitalWrite(LED_ACCESS_PIN, HIGH);
        lcd.print("Acceso exitoso.");

      } else {

        digitalWrite(LED_DENIED_PIN, HIGH);
        lcd.print("Acceso denegado.");
        Serial.println("\nUnknown card.") 
      }

      Serial.println("=============================================");

      mfrc522.PICC_HaltA();

      delay(3000);
      reset_state();
    }
  }

}

void reset_state()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Apoye su tarjeta.");

  digitalWrite(LED_ACCESS_PIN, LOW);
  digitalWrite(LED_DENIED_PIN, LOW);
}
