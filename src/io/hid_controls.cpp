#include "hid_controls.h"

#include <Arduino.h>

#if defined(CONFIG_TINYUSB_HID_ENABLED)
#include "USB.h"
#include "USBHIDConsumerControl.h"

static USBHIDConsumerControl ConsumerControl;
static bool hidReady = false;

static void sendConsumerControl(uint16_t code, int steps = 1) {
    if (!hidReady) {
        return;
    }

    steps = max(steps, 1);

    for (int i = 0; i < steps; i++) {
        ConsumerControl.press(code);
        delay(5);
        ConsumerControl.release();
        delay(5);
    }
}
#endif

void initHidControls() {
#if defined(CONFIG_TINYUSB_HID_ENABLED)
    USB.PID(0x1234);        // give it a unique product ID
    USB.manufacturerName("Ayden");
    USB.productName("PC Controller");
    ConsumerControl.begin();
    USB.begin();
    delay(100);             // give Windows time to enumerate
    hidReady = true;
#endif
}

void sendComputerVolumeUp(int steps) {
#if defined(CONFIG_TINYUSB_HID_ENABLED)
    sendConsumerControl(CONSUMER_CONTROL_VOLUME_INCREMENT, steps);
#else
    (void)steps;
#endif
}

void sendComputerVolumeDown(int steps) {
#if defined(CONFIG_TINYUSB_HID_ENABLED)
    sendConsumerControl(CONSUMER_CONTROL_VOLUME_DECREMENT, steps);
#else
    (void)steps;
#endif
}

void sendComputerMute() {
#if defined(CONFIG_TINYUSB_HID_ENABLED)
    sendConsumerControl(CONSUMER_CONTROL_MUTE);
#endif
}

void sendComputerPlayPause() {
#if defined(CONFIG_TINYUSB_HID_ENABLED)
    sendConsumerControl(CONSUMER_CONTROL_PLAY_PAUSE);
#endif
}

void sendComputerPrevious() {
#if defined(CONFIG_TINYUSB_HID_ENABLED)
    sendConsumerControl(CONSUMER_CONTROL_SCAN_PREVIOUS);
#endif
}

void sendComputerNext() {
#if defined(CONFIG_TINYUSB_HID_ENABLED)
    sendConsumerControl(CONSUMER_CONTROL_SCAN_NEXT);
#endif
}
