#ifndef PixelBoard_VK_H
#define PixelBoard_VK_H

#include <Arduino.h>

class PixelBoard_VK {
  private:
    const int pin;
    bool zustandTaster;
    bool letzterZustand;
    unsigned long letzteZeit;
    const unsigned long entprellZeit;

  public:
    // Konstruktor
    PixelBoard_VK(int pinTaster, unsigned long entprellZeitMs = 50);

    // Methode zur Aktualisierung des Tasterzustands
    void aktualisiere();

    // Gibt zurück, ob der Taster aktuell gedrückt ist
    bool istGedrueckt();

    // Gibt zurück, ob der Taster seit dem letzten Aufruf gedrückt wurde
    bool wurdeGedrueckt();
};

#endif