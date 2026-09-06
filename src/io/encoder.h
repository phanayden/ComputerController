#pragma once

#include <Arduino.h>

struct EncoderState {
  int pinA;
  int pinB;
  int pinSW;

  int delta;

  bool lastButtonReading;
  bool stableButtonState;
  bool buttonPressedEvent;
  unsigned long lastDebounceTime;

  uint8_t lastEncoded;
  int8_t encoderAccumulator;
};

void initEncoder(EncoderState &encoder, int pinA, int pinB, int pinSW);
void updateEncoder(EncoderState &encoder);

int getEncoderDelta(EncoderState &encoder);
bool encoderButtonPressed(EncoderState &encoder);