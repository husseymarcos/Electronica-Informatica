/*Adjunto codigo que posiblemente sirva para testear el sd.
 *Lo único que permitiría darte este código es mostrarte lo que dice el archivo.txt
 *Es necesario mencionar que el archivo.txt está ya subida en la micro.
*/

// Adjunto código
#include <SD.h>
#include <SPI.h>

File myFile;

const int SS_CS_PIN = 5; // También puede definirse así: #define SS_CS_PIN 5

void setup()
{
  Serial.begin(115200);
  Serial.print("Iniciando SD ...");

  if (!SD.begin(SS_CS_PIN)) {
    Serial.println("No se pudo inicializar");
    return;
  }

  Serial.print("Inicializacion exitosa");
 
  myFile = SD.open("archivo.txt"); //abrimos  el archivo 

  if(myFile){
    Serial.println("archivo.txt");
    while (myFile.available()) {
    	Serial.write(myFile.read());
    }

    myFile.close(); //cerramos el archivo
  } 
  else {
    Serial.println("Error al abrir el archivo");
  }
}

void loop() 
{

}
