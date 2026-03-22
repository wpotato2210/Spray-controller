# INSTALL

Spray Controller Installation

## Firmware Setup

- Toolchain: Arduino IDE
- Board: Arduino Nano
- Port: COMx or /dev/ttyUSBx
- Libraries: Servo.h and Wire.h

## Install Steps

1. Connect controller hardware per HARDWARE.md.
2. Open firmware project in Arduino IDE.
3. Select board and serial port.
4. Build and upload firmware.
5. Validate protocol output and I/O behavior per TESTING.md.

## Notes

- Use CONFIGURATION.md values for target rate and constants.
- Protocol framing and message formats are in PROTOCOLS.md.

## Placeholders

- Pinned library versions
- CI build artifact workflow
- Optional GUI integration
