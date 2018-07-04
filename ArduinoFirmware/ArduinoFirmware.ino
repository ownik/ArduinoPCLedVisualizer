#include "Adafruit_NeoPixel.h"

#define PIN 3        // пин DI
#define NUM_LEDS 30   // число диодов

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

unsigned char currentColor = 0;
unsigned char currentLED = 0;
unsigned char buffer[3] = {0, 0, 0};

void setup() {
  strip.begin();
  strip.setBrightness(255);    // яркость, от 0 до 255
  strip.clear();                          // очистить

  for (int i = 0; i < NUM_LEDS; i++ ) {   // от 2/3 до конца
    strip.setPixelColor(i, 255, 255, 255);     // залить синим
  }

  strip.show();                           // отправить на ленту
  Serial.begin(9600);
}

void loop() {
  if (currentColor == 3) {
    strip.setPixelColor(currentLED, buffer[0], buffer[1], buffer[2]);
    strip.show();
    currentColor = 0;
    currentLED++;
    if (currentLED >= NUM_LEDS) {
      currentLED = 0;
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    unsigned char inChar = (unsigned char)Serial.read();
    buffer[currentColor] = inChar;
    currentColor++;
  }
}

