#include "led_ring.h"
#include "config.h"

#include <Arduino.h>
#include <FastLED.h>

CRGB leds[NUM_LEDS];

// Default normal LED color
static CRGB currentLedColor = CRGB::Green;

void initLedRing() {
    FastLED.addLeds<WS2812B, LED_RING_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);

    fill_solid(leds, NUM_LEDS, CRGB::Blue);
    FastLED.show();
    delay(150);
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
    delay(150);

    FastLED.clear();
    FastLED.show();
}

void setLedRingVolume(int volume) {
    volume = constrain(volume, 0, 100);

    int ledsOn = map(volume, 0, 100, 0, NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; i++) {
        if (i < ledsOn) {
            leds[i] = currentLedColor;
        } else {
            leds[i] = CRGB::Black;
        }
    }

    FastLED.show();
}

void setLedRingColor(const CRGB &color, int volume) {
    currentLedColor = color;
    setLedRingVolume(volume);
}

void setLedRingBrightness(int brightness) {
    brightness = constrain(brightness, 0, 50);
    FastLED.setBrightness(brightness);
    FastLED.show();
}

void clearLedRing() {
    FastLED.clear();
    FastLED.show();
}

void mutedLED(bool muted, int volume) {
    volume = constrain(volume, 0, 100);

    int ledsOn = map(volume, 0, 100, 0, NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; i++) {
        if (i < ledsOn) {
            if (muted) {
                leds[i] = CRGB::Red;
            } else {
                leds[i] = currentLedColor;
            }
        } else {
            leds[i] = CRGB::Black;
        }
    }

    FastLED.show();
}
