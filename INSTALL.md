# INSTALL.md

Spray Controller Installation

## Firmware Setup

- Toolchain: Arduino IDE
- Board: Arduino Nano
- Port: COMx / /dev/ttyUSBx
- Libraries:
  - `U8g2` by olikraus (`v2.35.x` baseline validated)
  - Arduino core (`Arduino.h`)

## Install Steps

1. Connect the controller hardware per `HARDWARE.md`.
1. Open firmware project in Arduino IDE.
1. Install `U8g2` from Library Manager (search for `U8g2`).
1. Select board and serial port.
1. Build and upload firmware.
1. Validate protocol output and I/O behavior per `TESTING.md`.

## Notes

- Use `CONFIGURATION.md` values for target rate and controller constants.
- Protocol framing and message formats are defined in `PROTOCOLS.md`.

## Placeholders

- Pinned library versions
- CI build artifact workflow
- Optional GUI integration
