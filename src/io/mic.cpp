#include "mic.h"
#include "../config.h"

#include <Arduino.h>

static const int ADC_MAX_VALUE = 4095;
static const float ADC_REF_VOLTAGE = 3.3;

// How many samples to collect when measuring loudness
static const int LOUDNESS_SAMPLE_COUNT = 100;

void initMic() {
    pinMode(MIC_PIN, INPUT);

    // ESP32 ADC default is usually 12-bit, but this makes it explicit
    analogReadResolution(12);

    Serial.println("Microphone initialized");
}

int readMicRaw() {
    return analogRead(MIC_PIN);
}

float readMicVoltage() {
    int raw = readMicRaw();
    return (raw * ADC_REF_VOLTAGE) / ADC_MAX_VALUE;
}

int getMicP2P() {
    int minValue = ADC_MAX_VALUE;
    int maxValue = 0;

    for (int i = 0; i < LOUDNESS_SAMPLE_COUNT; i++) {
        int sample = readMicRaw();

        if (sample < minValue) {
            minValue = sample;
        }

        if (sample > maxValue) {
            maxValue = sample;
        }

        delayMicroseconds(100); // 10 kHz sampling
    }

    return maxValue - minValue;
}

int getMicLoudnessPercent() {
    int peakToPeak = getMicP2P();

    // You will tune 1200 after testing your mic
    int loudness = map(peakToPeak, 0, 2400, 0, 100);

    return constrain(loudness, 0, 100);
}