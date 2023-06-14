// Slave

#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <esp_now.h>


// Configuración de la tarjeta microSD
#define SD_CS_PIN 5  // Pin del chip select de la tarjeta microSD


typedef struct struct_message{
  int a;
} struct_message;

struct_message data;

void OnRecv(const uint8_t * mac, const uint8_t *incomingData, int len){
  memcpy(&data, incomingData, sizeof(data));
  Serial.print("Bytes recibidos: ");
  Serial.print(len);
  Serial.print("Datos recibidos: ");
  Serial.print(data.a);
  
}

void setup() {
  Serial.begin(115200);
  

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta microSD");
    return;
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error al iniciar ESP-NOW");
    return;
  }
    
    esp_now_register_recv_cb(OnRecv);

    // Obtener el UID de la tarjeta RFID
    String uid = "UID_DE_EJEMPLO";

    // Guardar el dato en la tarjeta microSD
    File dataFile = SD.open(uid + ".txt", FILE_APPEND);
    if (dataFile) { 
      dataFile.println(data.a);
      dataFile.close();
    } 
    else {
      Serial.println("Error al abrir el archivo");

    }
  }




void loop() 
{
  
}
