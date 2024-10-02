#include <PixelBoard_Joystick.h>

PixelBoard_Joystick taster(32);  // Beispiel: Taster an Pin 5

unsigned long letzteZeit = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  taster.aktualisiere();

  if (taster.istGedrueckt()) {
    Serial.println("Taster ist gedrückt.");
  }
  // Passiert nur alle 5s
  if (millis() > letzteZeit+5000){
    if (taster.wurdeGedrueckt()) {
      Serial.println("Taster wurde seit dem letzten Aufruf gedrückt.");
    }
    letzteZeit=millis();
  }
}
