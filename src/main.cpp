#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "io/led_ring.h"
#include "io/encoder.h"
#include "io/mic.h"
#include "io/buttons.h"
#include "io/screen.h"
#include "io/hid_controls.h"

int volume = 0;
bool muted = false;
bool micMuted = false;

unsigned long lastMicUpdateTime = 0;

EncoderState encoder;

int micSensitivity = 100; // 100% = normal sensitivity

enum ControllerMode {
  MODE_VOLUME,
  MODE_MIC_GAIN,
  MODE_LIGHTING,
  MODE_WAVEFORM
};

int ledBrightness = LED_BRIGHTNESS;

CRGB ledColors[] = {
    CRGB::Green,
    CRGB::Blue,
    CRGB::Purple,
    CRGB::Cyan,
    CRGB::White,
    CRGB::Yellow
};

int currentLedColorIndex = 0;
const int NUM_LED_COLORS = sizeof(ledColors) / sizeof(ledColors[0]);

ControllerMode currentMode = MODE_VOLUME;

const int NUM_MODES = 4;

int waveformScale = 100;       // 100% = normal waveform height
bool waveformFrozen = false;

//used to apply the read volume to the controller on startup
void applyComputerVolume(int newVolume) {
  volume = constrain(newVolume, 0, 100);

  if (muted) {
    mutedLED(true, volume);
  } else {
    setLedRingVolume(volume);
  }

  updateVolumeUI(volume);
}

//reads the command and looks for the number after VOL:, and sets the correct read volume level when first booting
//also reads if sound is muted or not
void handleHostCommand(const String &command) {
  if (command.startsWith("VOL:")) {
    applyComputerVolume(command.substring(4).toInt());
    Serial.print("VOL_OK:");
    Serial.println(volume);
    return;
  }

  if (command.startsWith("MUTE:")) {
    muted = command.substring(5).toInt() != 0;
    mutedLED(muted, volume);
    updateMutedUI(muted);
    Serial.print("MUTE_OK:");
    Serial.println(muted ? 1 : 0);
    return;
  }
}

//goes first, reads the charactesr sent from macos.py, and sends the sentence/command to handleHostCommand
void updateHostSerial() {
  static String command;

  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\n' || c == '\r') {
      if (command.length() > 0) {
        handleHostCommand(command);
        command = "";
      }
      continue;
    }

    if (command.length() < 32) {
      command += c;
    } else {
      command = "";
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("BOOT: setup started");

  Serial.println("BOOT: HID init");
  initHidControls();
  Serial.println("BOOT: LED ring init");
  Serial.flush();
  initLedRing();
  Serial.println("BOOT: encoder init");
  Serial.flush();
  initEncoder(encoder, ENCODER_A_PIN, ENCODER_B_PIN, ENCODER_SW_PIN);
  Serial.println("BOOT: mic init");
  Serial.flush();
  initMic();
  Serial.println("BOOT: buttons init");
  Serial.flush();
  initButtons();
  Serial.println("BOOT: screen init");
  Serial.flush();
  initScreen();

  Serial.println("BOOT: drawing UI");
  Serial.flush();
  setLedRingVolume(volume);
  updateVolumeUI(volume);
  updateMutedUI(muted);
  updateMicUI(0);
  updateWaveformUI(0);
  updateMicMuteButtonUI(micMuted);
  updateMicGainUI(micSensitivity);
  #if defined(CONFIG_TINYUSB_HID_ENABLED)
    Serial.println("HID enabled");
  #else
    Serial.println("HID NOT enabled - check build flags");
  #endif

  Serial.println("PC_CONTROLLER_READY");
  Serial.flush();
}

void updateMode() {
  if (toggleStatePressed()) {
    currentMode = (ControllerMode)((currentMode + 1) % NUM_MODES);

    updateModeUI((int)currentMode);

    Serial.print("Mode changed to: ");
    Serial.println(currentMode);
  }
}

void handleVolumeMode(int volumeDelta){
  if (volumeDelta != 0 && !muted) {
    volume += volumeDelta * 2;
    volume = constrain(volume, 0, 100);

    if (volumeDelta > 0) {
      sendComputerVolumeUp(volumeDelta * VOLUME_STEP);
    } else {
      sendComputerVolumeDown(-volumeDelta * VOLUME_STEP);
    }

    setLedRingVolume(volume);
    updateVolumeUI(volume);
  }

  if (encoderButtonPressed(encoder)) {
    muted = !muted;

    sendComputerMute();
    mutedLED(muted, volume);
    updateMutedUI(muted);

  }
}

void handleMicGainMode(int encoderDelta){
  if (encoderDelta != 0 && !micMuted) {
    micSensitivity += encoderDelta * MIC_SENSITIVITY_STEP;
    micSensitivity = constrain(micSensitivity, MIC_SENSITIVITY_MIN, MIC_SENSITIVITY_MAX);

    updateMicGainUI(micSensitivity);

    Serial.print("Mic sensitivity: ");
    Serial.print(micSensitivity);
    Serial.println("%");
  }
  if (encoderButtonPressed(encoder)) {
    micMuted = !micMuted;
    updateMicMuteButtonUI(micMuted);
    if (micMuted) {
      updateMicUI(0);
      updateWaveformUI(0);
    }
  }
}

void handleLightingMode(int delta) {
  if (delta != 0) {
    ledBrightness += delta * 5;
    ledBrightness = constrain(ledBrightness, 0, 255);

    setLedRingBrightness(ledBrightness);
    setLedRingColor(ledColors[currentLedColorIndex], volume);

    Serial.print("LED brightness: ");
    Serial.println(ledBrightness);
  }

  if (encoderButtonPressed(encoder)) {
    currentLedColorIndex = (currentLedColorIndex + 1) % NUM_LED_COLORS;

    setLedRingColor(ledColors[currentLedColorIndex], volume);

    Serial.print("LED color index: ");
    Serial.println(currentLedColorIndex);
  }
}

void handleWaveformMode(int delta) {
  if (delta != 0) {
    waveformScale += delta * 10;
    waveformScale = constrain(waveformScale, 50, 200);

    Serial.print("Waveform scale: ");
    Serial.print(waveformScale);
    Serial.println("%");
  }

  if (encoderButtonPressed(encoder)) {
    waveformFrozen = !waveformFrozen;

    Serial.print("Waveform frozen: ");
    Serial.println(waveformFrozen);
  }
}

void loop() {
  //updateHostSerial();
  updateEncoder(encoder);
  updateButtons();
  updateMode();
  int encoderDelta = getEncoderDelta(encoder); 

  switch (currentMode) {
    case MODE_VOLUME:
      handleVolumeMode(encoderDelta);
      break;

    case MODE_MIC_GAIN:
      handleMicGainMode(encoderDelta);
      break;

    case MODE_LIGHTING:
      handleLightingMode(encoderDelta);
      break;

    case MODE_WAVEFORM:
      handleWaveformMode(encoderDelta);
      break;
  }

  unsigned long now = millis();

  if (now - lastMicUpdateTime >= MIC_UPDATE_INTERVAL_MS) {
    lastMicUpdateTime = now;

    int micLoudness = getMicLoudnessPercent();

    int adjustedLoudness = micLoudness * micSensitivity / 100;
    adjustedLoudness = constrain(adjustedLoudness, 0, 100);

    static float smoothLoudness = 0;
    smoothLoudness = 0.85 * smoothLoudness + 0.15 * adjustedLoudness;

    if (smoothLoudness < 8) {
    smoothLoudness = 0;
    }

    int displayLoudness = (int)smoothLoudness;

    int scaledWaveformLoudness = displayLoudness * waveformScale / 100;
    scaledWaveformLoudness = constrain(scaledWaveformLoudness, 0, 100);

    if (!micMuted) {
      updateMicUI(displayLoudness);

      if (!waveformFrozen) {
        updateWaveformUI(scaledWaveformLoudness);
      }
    } else {
      updateMicUI(0);

      if (!waveformFrozen) {
        updateWaveformUI(0);
      }
    }
  }

  if (playPausePressed()) {
    sendComputerPlayPause();
    updatePlayPauseButtonUI(true);
    delay(80);
    updatePlayPauseButtonUI(false);
  }

  if (skipBackPressed()) {
    sendComputerPrevious();
    updateSkipBackButtonUI(true);
    delay(80);
    updateSkipBackButtonUI(false);
  }

  if (skipNextPressed()) {
    sendComputerNext();
    updateSkipNextButtonUI(true);
    delay(80);
    updateSkipNextButtonUI(false);
  }

  if (micMutePressed()) {
    micMuted = !micMuted;

    updateMicMuteButtonUI(micMuted);

    if (micMuted) {
      updateMicUI(0);
      updateWaveformUI(0);
    }
  }
}
