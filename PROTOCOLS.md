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
  - `bit4` = pressure sensor configuration invalid (only when pressure feature
    is enabled).
- `fault_text` is deterministic status text:
  - `OK` when `fault_bits == 0`.
  - `FAULT` when any fault bit is set.

### PREVIEW (P4 operator interface)

- Format:
- `PV:<speed_kmh>,<flow_lpm>,<pump_duty>,<active_sections>,<distance_m>,<area_ha>\n`
- Description:
- Fixed-cadence operator preview payload for menu and field visibility.

#### PREVIEW field contract

- `speed_kmh`: Wheel-derived speed in km/h (3 decimal precision).
- `flow_lpm`: Total measured flow in L/min (3 decimal precision).
- `pump_duty`: Current pump PWM duty in range `0-255`.
- `active_sections`: Count of enabled boom sections in range `0-3`.
- `distance_m`: Accumulated travel distance in meters (3 decimal precision).
- `area_ha`: Accumulated sprayed area in hectares (6 decimal precision).

#### PREVIEW cadence

- Publish at a deterministic fixed interval owned by main loop timing.
- Cadence target is tied to the canonical loop interval; no burst/backfill
  publishing is allowed.

### OPERATOR MENU STATE (P4 operator interface)

- Menu states are deterministic and finite:
  - `HOME`: Default view with live preview fields.
  - `MENU`: Top-level selection view.
  - `COUNTERS`: Distance/area view.
  - `RESET_CONFIRM`: Explicit destructive-action confirmation view.
- Allowed transitions:
  - `HOME -> MENU` (navigate)
  - `MENU -> HOME` (cancel/back)
  - `MENU -> COUNTERS` (select counters)
  - `COUNTERS -> MENU` (cancel/back)
  - `COUNTERS -> RESET_CONFIRM` (select reset)
  - `RESET_CONFIRM -> COUNTERS` (cancel)
  - `RESET_CONFIRM -> COUNTERS` (confirm reset complete)
- Any undefined transition request must be ignored and state preserved.

### RESET CONFIRM HANDSHAKE (P4 operator interface)

- Reset action is two-step and explicit:
  1. Enter `RESET_CONFIRM` from `COUNTERS` via a reset-select event.
  2. Accept only a `CONFIRM` event to execute reset.
- `CANCEL` must abort reset and return to `COUNTERS` with values unchanged.
- On successful `CONFIRM`, both `distance_m` and `area_ha` are set to zero in a
  single deterministic update cycle.

### PRESSURE (optional, compile-time gated)

- Enabled only when `ENABLE_PRESSURE_SENSOR=true` in `config.h` (default is
  `false`).
- Format:
- `PR:<pressure_kpa>\n`
- Description:
- Telemetry-only pressure reading in kPa (2 decimal precision). No control loop
  coupling.

## Versioning

- `PROTOCOL_V1`

## Placeholders

- Full byte-level serialization
- Acknowledgment/error messages
- Error codes/retry logic
