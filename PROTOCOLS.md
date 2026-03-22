# PROTOCOLS.md

Spray Controller Protocols

## Message Formats

### STATUS

- Format:
- `ST:<lpm>,<pump_duty>,<run>,<section_mask>,<fault_bits>,<fault_text>\n`
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
- `fault_bits` is an unsigned bitfield:
  - `bit0` = flow stale timeout active.
  - `bit1` = wheel pulse timeout active.
  - `bit2` = flow sensor configuration invalid.
  - `bit3` = wheel sensor configuration invalid.
- `fault_text` is deterministic status text:
  - `OK` when `fault_bits == 0`.
  - `FAULT` when any fault bit is set.

## Versioning

- `PROTOCOL_V1`

## Placeholders

- Full byte-level serialization
- Acknowledgment/error messages
- Error codes/retry logic
