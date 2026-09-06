#include "screen.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

static TFT_eSPI tft = TFT_eSPI();

void initScreen() {
    tft.init();
    tft.setRotation(1);

    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextSize(3);
    tft.setCursor(20, 20);
    tft.print("PC CONTROLLER");
    delay(250);

    tft.fillScreen(TFT_BLACK);

    // Waveform area at top
    tft.drawRect(10, 10, 460, 120, TFT_BLUE);

    // Static labels
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    tft.setCursor(20, 145);
    tft.print("Volume:");

    tft.setCursor(220, 145);
    tft.print("Mic:");

    tft.setCursor(20, 175);
    tft.print("Sound:");

    tft.setCursor(220, 175);
    tft.print("Gain:");

    tft.setCursor(20, 205);
    tft.print("Mode:");

    // Initial values
    updateVolumeUI(0);
    updateMicUI(0);
    updateMutedUI(false);
    updateMicGainUI(100);
    updateModeUI(0);

    // Button icons
    updatePlayPauseButtonUI(false);
    updateSkipBackButtonUI(false);
    updateSkipNextButtonUI(false);
    updateMicMuteButtonUI(false);
}

static void drawButtonIcon(int x, int y, const char* label, bool active, uint32_t activeColor) {
    uint32_t color = active ? activeColor : (uint32_t)TFT_DARKGREY;

    tft.fillRect(x - 45, y - 35, 90, 90, TFT_BLACK);
    tft.fillCircle(x, y, 24, color);
    tft.drawCircle(x, y, 24, TFT_WHITE);

    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    int textWidth = strlen(label) * 12;
    int labelX = x - textWidth / 2;
    int labelY = y + 32;

    tft.setCursor(labelX, labelY);
    tft.print(label);
}

void updatePlayPauseButtonUI(bool pressed) {
    drawButtonIcon(80, 268, "PLAY", pressed, TFT_GREEN);
}

void updateSkipBackButtonUI(bool pressed) {
    drawButtonIcon(185, 268, "BACK", pressed, TFT_GREEN);
}

void updateSkipNextButtonUI(bool pressed) {
    drawButtonIcon(290, 268, "NEXT", pressed, TFT_GREEN);
}

void updateMicMuteButtonUI(bool micMuted) {
    if (micMuted) {
        drawButtonIcon(395, 268, "MIC", true, TFT_RED);
    } else {
        drawButtonIcon(395, 268, "MIC", true, TFT_GREEN);
    }
}

void updateVolumeUI(int volume) {
    tft.fillRect(110, 145, 80, 22, TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(110, 145);
    tft.print(volume);
}

void updateMicUI(int micLoudness) {
    tft.fillRect(270, 145, 100, 22, TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(270, 145);
    tft.print(micLoudness);
    tft.print("%");
}

void updateMutedUI(bool muted) {
    tft.fillRect(105, 175, 100, 22, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(105, 175);

    if (muted) {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.print("ON");
    } else {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.print("OFF");
    }
}

void updateWaveformUI(int micLoudness) {
    const int x0 = 10;
    const int y0 = 10;
    const int width = 460;
    const int height = 120;

    const int usableWidth = width - 2;
    const int centerY = y0 + height / 2;
    const float periods = 4.0;

    static int prevY[458];
    static bool firstDraw = true;

    micLoudness = constrain(micLoudness, 0, 100);
    int amplitude = map(micLoudness, 0, 100, 5, 50);

    if (!firstDraw) {
        for (int x = 0; x < usableWidth - 1; x++) {
            tft.drawLine(x0 + 1 + x, prevY[x], x0 + 1 + x + 1, prevY[x + 1], TFT_BLACK);
        }
    }

    for (int x = 0; x < usableWidth; x++) {
        float angle = 2.0 * PI * periods * x / usableWidth;
        prevY[x] = centerY + amplitude * sin(angle);
    }

    for (int x = 0; x < usableWidth - 1; x++) {
        tft.drawLine(x0 + 1 + x, prevY[x], x0 + 1 + x + 1, prevY[x + 1], TFT_GREEN);
    }

    tft.drawRect(x0, y0, width, height, TFT_BLUE);
    firstDraw = false;
}

void updateMicGainUI(int micSensitivity) {
    micSensitivity = constrain(micSensitivity, 25, 200);
    tft.fillRect(285, 175, 120, 22, TFT_BLACK);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(285, 175);
    tft.print(micSensitivity);
    tft.print("%");
}

void updateModeUI(int mode) {
    tft.fillRect(90, 205, 250, 22, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(90, 205);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    switch (mode) {
        case 0: tft.print("VOLUME");   break;
        case 1: tft.print("MIC GAIN"); break;
        case 2: tft.print("LIGHTING"); break;
        case 3: tft.print("WAVEFORM"); break;
        default: tft.print("UNKNOWN"); break;
    }
}