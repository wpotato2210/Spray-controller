# PROTOCOLS.md

Spray Controller Protocols

## Message Formats

### STATUS

- Format:
  - `ST:<lpm>,<pump_duty>,<run>,<section_mask>\n`
- Description:
  - Deterministic status frame.

## Notes

- `lpm` is the measured total flow (3 decimal precision).
- `pump_duty` is the PWM duty in range `0-255`.
- `run` is `1` for RUN and `0` for HOLD.
- `section_mask` packs section states in LSB-first order:
  - `bit0=section0`
  - `bit1=section1`
  - `bit2=section2`

## Versioning

- `PROTOCOL_V1`

## Placeholders

- Full byte-level serialization
- Acknowledgment/error messages
- Error codes/retry logic
