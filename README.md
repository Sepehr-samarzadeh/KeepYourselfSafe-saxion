# Arduino Vault Project

## Overview

This Arduino project implements a code-based vault system using a rotary encoder, shift register, seven-segment display, bcd and ..., The vault can be programmed with a secret code, and users can input the code using the rotary encoder to access the vault.

## Components

- Arduino Uno
- Rotary Encoder
- SN74HC595N Shift Register
- Seven-Segment Display
- LEDs (for feedback)
- Buzzer
- Servo Motor
- BCD decoder

## Setup

1. Connect the components based on the provided wiring diagram.
2. Upload the Arduino code to the Arduino Uno.
3. Power on the system and test the vault.

## Features

- **Code Input:** Use the rotary encoder to input the secret code on the seven-segment display.
- **Feedback LEDs:** LEDs provide feedback on the correctness of the entered code.
- **Buzzer:** A buzzer signals correct and incorrect code entries.
- **Servo Motor:** The vault opens when the correct code is entered.
- **EEPROM Storage:** The code is stored in EEPROM, allowing it to persist through power cycles.


## Acknowledgments

- Arduino Community and Documentation
