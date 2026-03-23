# PROTOCOLS.md

Spray Controller Protocols

## Message Formats

### STATUS

- Format:
- `ST:<lpm>,<pump_duty>,<run>,<section_mask>,<fault_bits>,<fault_text>\n`
- Description:
- Deterministic status frame.

#### STATUS field contract

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

### SECTION TELEMETRY (P5 scalability mapping)

- Format:
- `S:<section_id>,<field_id>,<value>\n`
- Description:
- Deterministic per-section telemetry emitted for every configured section in
  ascending `section_id` order on each telemetry cadence.

#### SECTION ID contract

- Section IDs are stable compile-time identities sourced from
  `kSectionDescriptors`.
- Current canonical mapping:
  - `0` = section0
  - `1` = section1
  - `2` = section2
- Output ordering is strictly ascending by `section_id`.
- Each telemetry cycle emits exactly two `S:` frames per configured section:
  - `field_id=0` = output state currently driven by firmware.
  - `field_id=1` = operator switch state sampled from the section input.
- `value` is `1` when active/enabled and `0` when inactive/disabled.

### SENSOR TELEMETRY (P5 scalability mapping)

- Format:
- `SN:<sensor_id>,<field_id>,<value>[,<detail>]\n`
- Description:
- Deterministic per-sensor telemetry emitted in ascending `sensor_id` order on
  each telemetry cadence.

#### SENSOR ID contract

- Sensor IDs are stable compile-time identities defined in
  `include/protocol.h`.
- Canonical mapping:
  - `0` = total flow sensor
  - `1` = wheel speed sensor
  - `2` = pressure sensor (only emitted when pressure feature is enabled)
- Output ordering is strictly ascending by `sensor_id`.
- Each telemetry cycle emits exactly two `SN:` frames per emitted sensor:
  - `field_id=0` = primary measured value.
  - `field_id=1` = deterministic fault detail bits for that sensor.
- Primary value contract:
  - `sensor_id=0`: `value=<flow_lpm>` with 3 decimal precision.
  - `sensor_id=1`: `value=<speed_kmh>` with 3 decimal precision.
  - `sensor_id=2`: `value=<pressure_kpa>` with 2 decimal precision.
- Fault detail contract:
  - `sensor_id=0`: `value=<stale_fault>,<config_fault>`.
  - `sensor_id=1`: `value=<timeout_fault>,<config_fault>`.
  - `sensor_id=2`: `value=<config_fault>`.
- Fault detail values are binary (`0` or `1`) and preserve fixed field order.

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

#### MENU command input

- Format:
- `ME:<event>\n`
- Accepted event tokens:
  - `NAV` = navigate
  - `SEL` = select
  - `CAN` = cancel/back
  - `CFM` = confirm
- Unknown tokens are ignored with no state change.

#### MENU state output

- Format:
- `MS:<state>\n`
- Output is emitted when a valid menu event is accepted and processed at the
  deterministic menu cadence.

### RESET CONFIRM HANDSHAKE (P4 operator interface)

- Reset action is two-step and explicit:
  1. Enter `RESET_CONFIRM` from `COUNTERS` via a reset-select event.
  2. Accept only a `CONFIRM` event to execute reset.
- `CANCEL` must abort reset and return to `COUNTERS` with values unchanged.
- On successful `CONFIRM`, both `distance_m` and `area_ha` are set to zero in a
  single deterministic update cycle.
- Successful reset also clears runtime calibration/sensor state via module
  reset hooks and emits a deterministic reset event frame.

#### RESET event output

- Format:
- `RS:COUNTERS_CALIBRATION_RESET\n`
- Emitted exactly once per successful `CONFIRM` reset action.

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
