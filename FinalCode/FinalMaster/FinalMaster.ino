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

// Configuración de la tira de LED
#define LED_PIN 15
#define LED_COUNT 1
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Dirección MAC del ESP32 esclavo
uint8_t slaveAddress[] = {0xC8, 0xF0, 0x9E, 0x53, 0x05, 0xD8};

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Iniciar comunicación SPI
  mfrc522.PCD_Init(); // Iniciar el lector RFID
  lcd.begin(16, 2); // Iniciar el LCD
  strip.begin(); // Iniciar la tira de LED
  strip.setPixelColor(0, 0, 255, 0); // Establecer color verde al LED
  strip.show(); // Mostrar el color en el LED

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al inicializar ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER); // Configurar ESP-NOW como controlador
  esp_now_add_peer(slaveAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0); // Agregar el ESP32 esclavo

  // Función de callback para recibir datos del esclavo
  esp_now_register_recv_cb([](const uint8_t* mac, const uint8_t* data, int len) {
    Serial.print("Datos recibidos del esclavo: ");
    for (int i = 0; i < len; i++) {
      Serial.print((char)data[i]);
    }
    Serial.println();
  });
}

