#include <FastLED.h>

#define NUM_LEDS 256
#define WIDTH 32
#define HEIGHT 16

#define LEDS1_PIN 26
#define LEDS2_PIN 25

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812B, LEDS1_PIN, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812B, LEDS2_PIN, GRB>(leds2, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.clear(true);
  FastLED.show();

  pixelTest();
}

void loop() {
  // leer – der Test läuft nur einmal im Setup
}

// Konvertiert x/y in LED-Index für serpentinierte Matrix
int getLedIndex(int x, int y) {
  int index = x * 8;
  if (x % 2 == 0) {
    index += 7 - y;
  } else {
    index += y;
  }
  return index;
}

// Setzt eine LED auf Farbe abhängig davon, ob sie zu Matrix 1 oder 2 gehört
void setLed(int x, int y, CRGB color) {
  if (y < 8) {
    leds1[getLedIndex(31 - x, 7 - y)] = color;  // untere Hälfte (y: 0–7)
  } else {
    leds2[getLedIndex(x, y - 8)] = color;       // obere Hälfte (y: 8–15)
  }
  FastLED.show();
}

// Testet jede LED nacheinander
void pixelTest() {
  FastLED.clear(true);
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      setLed(x, y, CRGB::White);
      delay(30);
      setLed(x, y, CRGB::Black);
    }
  }
  FastLED.show();
}
