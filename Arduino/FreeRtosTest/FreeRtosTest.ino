#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <FastLED.h>

// WLAN-Zugangsdaten
const char* ssid = "Test";
const char* password = "ImpfenIsCool";

// OpenWeatherMap API Einstellungen
const char* apiKey = "1054e4026039a886c5cf52e08637adcc";
const char* city = "Innsbruck";
const char* countryCode = "AT";
String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "," + String(countryCode) + "&appid=" + apiKey + "&units=metric";

// NTP-Server für Zeitabfrage
const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 3600;
int daylightOffset_sec = 3600;

// LED-Panel Definition
#define LED_PIN 25
#define NUM_LEDS 512
#define PANEL_WIDTH 32
#define PANEL_HEIGHT 16
CRGB leds[NUM_LEDS];

// Funktion zum Setzen einer LED mit x/y-Koordinaten für 2 gespiegelte Panels (jeweils 8x32 übereinander)
void setLED(int8_t x, int8_t y, CRGB color) {
    if (x < 0 || x >= 32 || y < 0 || y >= 16) { // Bereichsanpassung auf 32x16
        Serial.printf("Ungültige Koordinaten: x=%d, y=%d\n", x, y);
        return;
    }

    uint16_t index;
    uint8_t col = 31 - x;  // X-Achse gespiegelt (von rechts nach links)

    if (y < 8) {  
        // Unteres Panel (0-7) - bleibt gleich
        index = col * 8;
        if (col % 2 == 0) {
            index += (7 - y);  // Gerade Spalten: oben nach unten
        } else {
            index += y;        // Ungerade Spalten: unten nach oben
        }
    } else {  
        // Oberes Panel (8-15) - Jetzt RICHTIG in der Y-Achse gespiegelt!
        index = 256 + (col * 8);
        if (col % 2 == 0) {
            index += (15 - y);  // Gerade Spalten: oben nach unten
        } else {
            index += (y - 8);   // Ungerade Spalten: unten nach oben
        }
    }

    leds[index] = color;
    FastLED.show();
}

// Wetter-Task
void getWeatherTask(void *pvParameters) {
    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(weatherUrl);
            int httpResponseCode = http.GET();

            if (httpResponseCode > 0) {
                String payload = http.getString();
                StaticJsonDocument<1024> doc;
                deserializeJson(doc, payload);
                float temp = doc["main"]["temp"];
                const char* weather = doc["weather"][0]["description"];

                Serial.print("Wetter in ");
                Serial.print(city);
                Serial.print(": ");
                Serial.print(temp);
                Serial.print("°C, ");
                Serial.println(weather);
            } else {
                Serial.print("Fehler beim Abrufen der Wetterdaten: ");
                Serial.println(httpResponseCode);
            }
            http.end();
        }
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}

// Zeit-Task
void getTimeTask(void *pvParameters) {
    while (1) {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Fehler beim Abrufen der Zeit");
        } else {
            Serial.print("Aktuelle Zeit: ");
            Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

// Wi-Fi verbinden
void connectWiFi() {
    Serial.print("Verbinde mit WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println(" Verbunden!");
}

// Setup-Funktion
void setup() {
    Serial.begin(115200);
    connectWiFi();
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // LEDs initialisieren
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    FastLED.clear();
    FastLED.show();

    // FreeRTOS Tasks starten
    xTaskCreate(getWeatherTask, "WetterTask", 4096, NULL, 1, NULL);
    xTaskCreate(getTimeTask, "ZeitTask", 4096, NULL, 1, NULL);
}

// Loop durchläuft alle LEDs von x=0-31 und y=0-15 einzeln (mit Ausschalten)
void loop() {
    for (int x = 0; x < 32; x++) {  // 32 Spalten
        for (int y = 0; y < 16; y++) {  // 16 Reihen (8 pro Panel)
            setLED(x, y, CRGB::Blue); // LED anmachen
            delay(50);                // Kurze Verzögerung
            setLED(x, y, CRGB::Black); // LED wieder ausmachen
            delay(20);                 // Kurze Verzögerung
        }
    }

    delay(1000); // 1 Sekunde Pause nach einem kompletten Durchlauf
}
