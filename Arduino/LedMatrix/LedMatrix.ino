#include <FastLED.h>
 
#define LED_PIN     25
#define NUM_LEDS    512
#define PANEL_WIDTH 32
#define PANEL_HEIGHT 16
 
CRGB leds[NUM_LEDS];
 
void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
}
 
int getPixel(int x, int y) {
    int k = x / 2; 
    int d = x % 2;   
    
    if (y < 8) {
        int r = y;
        if (d == 0)
            return r + 16 * k;
        else
            return (15 - r) + 16 * k;
    } else {    
        int r = 15 - y; 
        int bottom_val;
        if (d == 0)
            bottom_val = r + 16 * k;
        else
            bottom_val = (15 - r) + 16 * k;
        return 511 - bottom_val;
    }
}

 
void loop() {
    for (int y = 0; y < PANEL_HEIGHT; y++) {
        for (int x = 0; x < PANEL_WIDTH; x++) {
            int index = getPixel(x, y);
            
            leds[index] = CRGB::White;
            FastLED.show();
            delay(20);

            leds[index] = CRGB::Black;
            FastLED.show();
        }
    }
}
