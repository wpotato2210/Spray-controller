# INSTALL.md

Spray Controller Installation

## Firmware Setup

- Toolchain: Arduino IDE
- Board: Arduino Nano
- Port: COMx / /dev/ttyUSBx
- Libraries: Servo.h, Wire.h

## Install Steps

1. Connect the controller hardware per `HARDWARE.md`.
2. Open firmware project in Arduino IDE.
3. Select board and serial port.
4. Build and upload firmware.
5. Validate protocol output and I/O behavior per `TESTING.md`.

## Notes

- Use `CONFIGURATION.md` values for target rate and controller constants.
- Protocol framing and message formats are defined in `PROTOCOLS.md`.

## Placeholders

- Pinned library versions
- CI build artifact workflow
- Optional GUI integration
