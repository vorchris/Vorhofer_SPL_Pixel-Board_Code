#include <FastLED.h>

#define LED_PIN_1     25
#define LED_PIN_2     26
#define BUTTON_PIN    32
#define NUM_LEDS      512
#define LONG_PRESS_DURATION 200  // in ms

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];

bool buttonPressed = false;
bool lastButtonPressed = false;
unsigned long pressStartTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Taster gegen GND

  FastLED.addLeds<WS2812B, LED_PIN_1, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_2, GRB>(leds2, NUM_LEDS);
  
  FastLED.setBrightness(50);
  FastLED.clear(true);
}

void loop() {
  // --- Button Handling ---
  buttonPressed = digitalRead(BUTTON_PIN) == LOW;

  if (buttonPressed && !lastButtonPressed) {
    pressStartTime = millis();
    fillAllRed();
  }
  else if (!buttonPressed && lastButtonPressed) {
    unsigned long duration = millis() - pressStartTime;
    FastLED.clear(true);

    if (duration >= LONG_PRESS_DURATION) {
      Serial.println("a");  // langer Druck
    } else {
      Serial.println("b");  // kurzer Druck
    }
  }

  lastButtonPressed = buttonPressed;

  // --- Serial Input Handling ---
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == 'a') {
      flashGreen(200);
    } else if (input == 'b') {
      flashGreen(50);
    }
  }
}

void fillAllRed() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds1[i] = CRGB::Red;
    leds2[i] = CRGB::Red;
  }
  FastLED.show();
}

void flashGreen(unsigned long duration) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds1[i] = CRGB::Green;
    leds2[i] = CRGB::Green;
  }
  FastLED.show();
  delay(duration);
  FastLED.clear(true);
}
