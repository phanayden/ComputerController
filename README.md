# ComputerController
Custom Computer Controller that uses a custom PCB to control the media functions of the device it is connected to, as well as view the loudness of the room. 

## Key Project Features
* MODE switching button that changes the use of the rotary encoder between VOLUME, MIC GAIN, BRIGHTNESS, and MIC SENSITIVITY
* PLAY, NEXT, BACK, MUTE MIC buttons
* LCD Screen that shows the current volume, mic loudness and gain, mode, if the volume is muted, if the mic is muted, and when each button is pressed
* Each component has a coupling and bulk capacitor
* LED Ring that displays the current volume level in green, and turns red when muted

## Components
* **Microcontroller:** ESP32-S3 N16R8 Development Board
* **Display:** Hosyond 3.5 Inches TFT LCD Touch Screen Shield Display Module 480x320 SPI Serial ILI9488
* **Microphone:** Teyleten Robot MAX9814 Electret Microphone
* **Rotary Encoder:** EC11 Rotary Encoder with Push Button
* **Switches:** GATERON Creamy Keyboard Switches
* **LED Ring:** WS2812B RGB LED Rings 12 Bits

## Hardware Architecture and Visuals
