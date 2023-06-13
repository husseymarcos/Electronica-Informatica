#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <SPI.h>
#include <esp_now.h>
#include <Adafruit_NeoPixel.h>

#define SS_PIN 5  // Pin del módulo RC522
#define RST_PIN 22  // Pin de reset del módulo RC522

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Crear instancia del lector RFID

typedef struct struct_message {
  int a;
} struct_message;

struct_message myData;

// Configuración del Keypad
const byte ROWS = 4; // Número de filas
const byte COLS = 4; // Número de columnas
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {23, 19, 18, 5}; // Conexiones de los pines de las filas
byte colPins[COLS] = {17, 16, 4, 2}; // Conexiones de los pines de las columnas
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Configuración del LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Falta el codigo de los leds

// Dirección MAC del ESP32 esclavo
uint8_t slaveAddress[] = {0xC8, 0xF0, 0x9E, 0x53, 0x05, 0xD8};

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Iniciar comunicación SPI
  mfrc522.PCD_Init(); // Iniciar el lector RFID
  lcd.begin(16, 2); // Iniciar el LCD
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }

  esp_now_add_peer(slaveAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0); // Agregar el ESP32 esclavo
}

