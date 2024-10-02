#include "PixelBoard_VK.h"

PixelBoard_VK::PixelBoard_VK(int pinTaster, unsigned long entprellZeitMs)
  : pin(pinTaster), zustandTaster(HIGH), letzterZustand(HIGH),
    letzteZeit(0), entprellZeit(entprellZeitMs) {
  pinMode(pin, INPUT_PULLUP);
}

void PixelBoard_VK::aktualisiere() {
  bool leseWert = digitalRead(pin);
  unsigned long aktuelleZeit = millis();

  if (leseWert != letzterZustand) {
    letzteZeit = aktuelleZeit;
  }

  if ((aktuelleZeit - letzteZeit) > entprellZeit) {
    if (leseWert != zustandTaster) {
      zustandTaster = leseWert;
    }
  }

  letzterZustand = leseWert;
}

bool PixelBoard_VK::istGedrueckt() {
  return zustandTaster == LOW;
}

bool PixelBoard_VK::wurdeGedrueckt() {
  static bool letzterRückgabewert = false;
  bool aktuellGedrueckt = (zustandTaster == LOW);

  bool gedrücktSeitLetztemAufruf = aktuellGedrueckt && !letzterRückgabewert;

  letzterRückgabewert = aktuellGedrueckt;
  return gedrücktSeitLetztemAufruf;
}
