#include "encoder.h"
#include "../config.h"

#include <Arduino.h>

static const unsigned long DEBOUNCE_MS = 50;

void initEncoder(EncoderState &encoder, int pinA, int pinB, int pinSW) {
  // Save pin numbers into this specific encoder object
  encoder.pinA = pinA;
  encoder.pinB = pinB;
  encoder.pinSW = pinSW;

  pinMode(encoder.pinA, INPUT_PULLUP);
  pinMode(encoder.pinB, INPUT_PULLUP);
  pinMode(encoder.pinSW, INPUT_PULLUP);

  int a = digitalRead(encoder.pinA);
  int b = digitalRead(encoder.pinB);

  encoder.lastEncoded = (a << 1) | b;
  encoder.encoderAccumulator = 0;
  encoder.delta = 0;

  encoder.lastButtonReading = digitalRead(encoder.pinSW);
  encoder.stableButtonState = encoder.lastButtonReading;
  encoder.buttonPressedEvent = false;
  encoder.lastDebounceTime = 0;
}

// Updates one encoder.
// Valid quadrature sequence changes one bit at a time.
void updateEncoder(EncoderState &encoder) {
  int a = digitalRead(encoder.pinA);
  int b = digitalRead(encoder.pinB);

  uint8_t encoded = (a << 1) | b;
  uint8_t transition = (encoder.lastEncoded << 2) | encoded;

  // Valid quadrature transitions for one direction
  if (transition == 0b1101 ||
      transition == 0b0100 ||
      transition == 0b0010 ||
      transition == 0b1011) {
    encoder.encoderAccumulator++;
  }

  // Valid quadrature transitions for the other direction
  if (transition == 0b1110 ||
      transition == 0b0111 ||
      transition == 0b0001 ||
      transition == 0b1000) {
    encoder.encoderAccumulator--;
  }

  encoder.lastEncoded = encoded;

  // Most EC11 encoders produce about 4 transitions per physical click.
  if (encoder.encoderAccumulator >= 4) {
    encoder.delta++;
    encoder.encoderAccumulator = 0;
  }

  if (encoder.encoderAccumulator <= -4) {
    encoder.delta--;
    encoder.encoderAccumulator = 0;
  }

  // Button debounce
  bool reading = digitalRead(encoder.pinSW);

  if (reading != encoder.lastButtonReading) {
    encoder.lastDebounceTime = millis();
  }

  if ((millis() - encoder.lastDebounceTime) > DEBOUNCE_MS) {
    if (reading != encoder.stableButtonState) {
      encoder.stableButtonState = reading;

      // INPUT_PULLUP means pressed = LOW
      if (encoder.stableButtonState == LOW) {
        encoder.buttonPressedEvent = true;
      }
    }
  }

  encoder.lastButtonReading = reading;
}

int getEncoderDelta(EncoderState &encoder) {
  int delta = encoder.delta;
  encoder.delta = 0;
  return delta;
}

bool encoderButtonPressed(EncoderState &encoder) {
  if (encoder.buttonPressedEvent) {
    encoder.buttonPressedEvent = false;
    return true;
  }

  return false;
}