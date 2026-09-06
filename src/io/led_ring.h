#pragma once

#include <FastLED.h>

void initLedRing();
void setLedRingVolume(int volume);
void setLedRingColor(const CRGB &color, int volume);
void setLedRingBrightness(int brightness);
void clearLedRing();
void mutedLED(bool muted, int volume);
