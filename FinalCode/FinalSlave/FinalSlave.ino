#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <ESPNowP2P.h>

// Configuración de la tarjeta microSD
#define SD_CS_PIN 5  // Pin del chip select de la tarjeta microSD

void setup() {
  Serial.begin(115200);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta microSD");
    return;
  }

  if (!ESPNowP2P.begin()) {
    Serial.println("Error al iniciar ESPNowP2P");
    return;
  }

    // Obtener el UID de la tarjeta RFID
    String uid = "UID_DE_EJEMPLO";

    // Guardar el dato en la tarjeta microSD
    File dataFile = SD.open(uid + ".txt", FILE_APPEND);
    if (dataFile) {
      dataFile.println((char*)data);
      dataFile.close();
    } else {
      Serial.println("Error al abrir el archivo");
    }
  });

  ESPNowP2P.receive();
}

void loop() {
  // Tu código de loop aquí
}
