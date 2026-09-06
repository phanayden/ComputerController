#include "buttons.h"
#include "../config.h"

#include <Arduino.h>

struct Button {
    int pin;
    bool lastReading;
    bool stableState;
    bool pressedEvent;
    unsigned long lastDebounceTime;
};

static const unsigned long DEBOUNCE_MS = 50;

static Button playPauseButton = {PLAY_PAUSE_BUTTON_PIN, HIGH, HIGH, false, 0};
static Button skipBackButton  = {SKIP_BACK_BUTTON_PIN,  HIGH, HIGH, false, 0};
static Button skipNextButton  = {SKIP_NEXT_BUTTON_PIN,  HIGH, HIGH, false, 0};
static Button micMuteButton   = {MIC_MUTE_BUTTON_PIN,   HIGH, HIGH, false, 0};
static Button toggleStateButton = {MODE_BUTTON_PIN, HIGH, HIGH, false, 0};

static void initButton(Button &button) {
    pinMode(button.pin, INPUT_PULLUP); //change this for pcb to INPUT

    button.lastReading = digitalRead(button.pin);
    button.stableState = button.lastReading;
    button.pressedEvent = false;
    button.lastDebounceTime = 0;
}

static void updateButton(Button &button) {
    bool reading = digitalRead(button.pin);

    if (reading != button.lastReading) {
        button.lastDebounceTime = millis();
    }

    if ((millis() - button.lastDebounceTime) > DEBOUNCE_MS) {
        if (reading != button.stableState) {
            button.stableState = reading;

            // INPUT_PULLUP means pressed = LOW
            if (button.stableState == LOW) {
                button.pressedEvent = true;
            }
        }
    }

    button.lastReading = reading;
}

static bool consumePressedEvent(Button &button) {
    if (button.pressedEvent) {
        button.pressedEvent = false;
        return true;
    }

    return false;
}

void initButtons() {
    initButton(playPauseButton);
    initButton(skipBackButton);
    initButton(skipNextButton);
    initButton(micMuteButton);
    initButton(toggleStateButton);
}

void updateButtons() {
    updateButton(playPauseButton);
    updateButton(skipBackButton);
    updateButton(skipNextButton);
    updateButton(micMuteButton);
    updateButton(toggleStateButton);
}

bool playPausePressed() {
    return consumePressedEvent(playPauseButton);
}

bool skipBackPressed() {
    return consumePressedEvent(skipBackButton);
}

bool skipNextPressed() {
    return consumePressedEvent(skipNextButton);
}

bool micMutePressed() {
    return consumePressedEvent(micMuteButton);
}

bool toggleStatePressed() {
    return consumePressedEvent(toggleStateButton);
}