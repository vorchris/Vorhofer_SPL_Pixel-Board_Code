#include <FastLED.h>
 
#define LED_PIN     25
#define NUM_LEDS    512
#define PANEL_WIDTH 32
#define PANEL_HEIGHT 8
 
CRGB leds[NUM_LEDS];
 
void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
}
 
void setLED(int8_t x, int8_t y, CRGB color) {
    if (x < 0 || x > 63 || y < 0 || y > 7) {
        Serial.printf("Ungültige Koordinaten: x=%d, y=%d\n", x, y);
        return;
    }
   
    uint16_t index;
   
    if (x < PANEL_WIDTH) {  // Zweites Panel (0-31)
        uint8_t col = x;
        index = col * 8;
       
        if (col % 2 == 0) {
            index += (7 - y);  // Gerade Spalten: oben nach unten
        } else {
            index += y;        // Ungerade Spalten: unten nach oben
        }
    } else {  // Erstes Panel (32-63)
        x = x - 32;  // Normalisiere x für erstes Panel
        uint8_t col = 31 - x;
        index = 256 + (col * 8);
       
        if (col % 2 == 0) {
            index += y;        // Gerade Spalten: unten nach oben
        } else {
            index += (7 - y);  // Ungerade Spalten: oben nach unten
        }
    }
   
    leds[index] = color;
    FastLED.show();
}
 
void loop() {
    // Test Koordinaten
    setLED(0, 0, CRGB::Red);     // Unten links Panel 2
    setLED(31, 7, CRGB::Green);  // Oben rechts Panel 2
    setLED(32, 0, CRGB::Blue);   // Unten links Panel 1
    setLED(63, 7, CRGB::Yellow); // Oben rechts Panel 1
}