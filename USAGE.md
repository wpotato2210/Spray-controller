# USAGE.md

Spray Controller Usage Instructions

## Operation Steps

1. Power on the system.
1. Set the run/hold switch:

- Run: normal operation
- Hold: stops pump; optional bypass solenoid activated if configured

1. Enable or disable boom sections as needed.
1. Observe LED indicators for each active boom section.

## How Rate Control Works

- The controller maintains target application rate (L/ha) using vehicle speed

(km/h) and active spray width (m).

- Active spray width is the sum of ON boom sections (`SECTION_WIDTH_M` per

active section).

- Section changes affect effective width only; flow remains globally controlled

through one pump and one total flow sensor.

## Notes

- Flow calibration required before first operation.
- Safety interlocks should be verified prior to field use.

## Placeholders

- Specific calibration values for `TARGET_RATE_LPHA` and `KP`
- Safety and alarm behavior details
- Optional operator interface guidance
