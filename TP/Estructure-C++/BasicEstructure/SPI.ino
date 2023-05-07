#include <SPI.h>

const int CS_PIN = 5;  // El pin CS se conecta al pin 5 del ESP32

void setup() {
  Serial.begin(9600);
  SPI.begin();
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  // Desactivar el dispositivo en el arranque
}

void loop() {
  digitalWrite(CS_PIN, LOW);  // Activar el dispositivo
  SPI.transfer(0x02);         // Enviar el comando de lectura
  byte data = SPI.transfer(0x00);  // Leer el byte de datos
  digitalWrite(CS_PIN, HIGH);  // Desactivar el dispositivo

  Serial.print("Dato leído: ");
  Serial.println(data);

  delay(1000);
}

/*Este permite que haya comunicación entre el esclavo y el maestro. Considero que podría estar en ESP NOW
en mi código lo utilizo porque necesito que se realice la transferencia entre los esp.

El codigo aqui adjunto, solo es una muestra. Se debe adaptar a lo que nosotros queremos hacer.
*/