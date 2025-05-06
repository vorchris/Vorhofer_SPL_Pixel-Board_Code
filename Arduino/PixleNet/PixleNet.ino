#include <FastLED.h>

#define LED_PIN     25
#define BUTTON_PIN  32
#define NUM_LEDS    512
#define LONG_PRESS_DURATION 200  // in ms

CRGB leds[NUM_LEDS];

bool buttonPressed = false;
bool lastButtonPressed = false;
unsigned long pressStartTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Taster gegen GND
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  buttonPressed = digitalRead(BUTTON_PIN) == LOW;

  if (buttonPressed && !lastButtonPressed) {
    // Taste wurde gedrückt
    pressStartTime = millis();
    fillAllRed();
  }
  else if (!buttonPressed && lastButtonPressed) {
    // Taste wurde losgelassen
    unsigned long duration = millis() - pressStartTime;
    FastLED.clear();
    FastLED.show();

    if (duration >= LONG_PRESS_DURATION) {
      Serial.println("a");  // langer Druck
    } else {
      Serial.println("b");  // kurzer Druck
    }
  }

  lastButtonPressed = buttonPressed;
}

void fillAllRed() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
  }
  FastLED.show();
}
