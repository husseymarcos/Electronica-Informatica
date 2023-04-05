const int LedPin=2;
void setup() {
 // inicializa el pin LedPin como una salida.
 pinMode(LedPin, OUTPUT);
}
// Funcion LOOP que se repite hasta que se corta la energía
void loop() {
 digitalWrite(LedPin, HIGH); // Enciende el led pone el pin LedPin en HIGH
 delay(1000); // Espera un segundo
 digitalWrite(LedPin, LOW); // Apaga el led pone el pin LedPin en LOW
 delay(1000); // Espera un segundo
}