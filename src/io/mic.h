#pragma once

void initMic();

int readMicRaw();
float readMicVoltage();

int getMicP2P();
int getMicLoudnessPercent();