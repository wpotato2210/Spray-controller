# PROTOCOLS.md

Spray Controller Protocols

## Message Formats

### STATUS

- Format:
- `ST:<lpm>,<pump_duty>,<run>,<section_mask>,<fault_bits>,<fault_text>\n`
- Description:
- Deterministic status frame emitted on the telemetry cadence.

#### STATUS field contract

- `lpm` is the measured total flow in L/min with 3 decimal places.
- `pump_duty` is the PWM duty in range `0-255`; firmware publishes the commanded pump duty.
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
  - `bit4` = pressure sensor configuration invalid when `ENABLE_PRESSURE_SENSOR=true`.
- `fault_text` is deterministic status text:
  - `OK` when `fault_bits == 0`.
  - `FAULT` when any fault bit is set.

### SECTION TELEMETRY (P5 scalability mapping)

- Format:
- `S:<section_id>,<field_id>,<value>\n`
- Description:
- Deterministic per-section telemetry emitted for every configured section in ascending `section_id` order on each telemetry cadence.

#### SECTION ID contract

- Section IDs are stable compile-time identities sourced from
  `kSectionDescriptors`; `section_id` must equal the descriptor `id` field and
  must never be renumbered for an existing physical section.
- Current canonical mapping:
  - `0` = section0
  - `1` = section1
  - `2` = section2
- Output ordering is strictly ascending by `section_id`; firmware emits frames
  by iterating `kSectionDescriptors` in descriptor order, which must stay
  sorted by ascending `id`.
- Each telemetry cycle emits exactly two `S:` frames per configured section:
  - `field_id=0` = output state currently driven by firmware.
  - `field_id=1` = operator switch state sampled from the section input.
- `value` is `1` when active/enabled and `0` when inactive/disabled.

### SENSOR TELEMETRY (P5 scalability mapping)

- Format:
- `SN:<sensor_id>,<field_id>,<value>[,<detail>]\n`
- Description:
- Deterministic per-sensor telemetry emitted in ascending `sensor_id` order on each telemetry cadence.

#### SENSOR ID contract

- Sensor IDs are stable compile-time identities defined in
  `include/protocol.h` via `kTelemetrySensorContracts`; existing IDs must never
  be renumbered and new IDs append to the ordered contract.
- Canonical mapping:
  - `0` = total flow sensor
  - `1` = wheel speed sensor
  - `2` = pressure sensor (only emitted when pressure feature is enabled)
- Output ordering is strictly ascending by `sensor_id`; firmware emits frames by iterating `kTelemetrySensorContracts` in declared order.
- Each telemetry cycle emits exactly two `SN:` frames per emitted sensor by iterating the fixed field contract `kSensorTelemetryFrameContract`:
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

- `speed_kmh`: Wheel-derived speed in km/h with 3 decimal places.
- `flow_lpm`: Total measured flow in L/min with 3 decimal places.
- `pump_duty`: Current pump PWM duty in range `0-255`.
- `active_sections`: Count of enabled boom sections in range `0-3`.
- `distance_m`: Accumulated travel distance in meters with 3 decimal places.
- `area_ha`: Accumulated sprayed area in hectares with 6 decimal places.

#### PREVIEW cadence

- Publish at the fixed main-loop cadence owned by `PREVIEW_INTERVAL_MS`.
- Default cadence equals `LOOP_INTERVAL_MS`, which is `50 ms` in `config.h`.
- No burst or backfill publishing is allowed.

### OPERATOR MENU STATE (P4 operator interface)

- Menu states are deterministic and finite:
  - `HOME`: Default view with live preview fields.
  - `MENU`: Top-level selection view.
  - `COUNTERS`: Distance/area view.
  - `RESET_CONFIRM`: Explicit destructive-action confirmation view.
- Allowed transitions:
  - `HOME -> MENU` via `NAV`
  - `MENU -> HOME` via `CAN`
  - `MENU -> COUNTERS` via `SEL`
  - `COUNTERS -> MENU` via `CAN`
  - `COUNTERS -> RESET_CONFIRM` via `SEL`
  - `RESET_CONFIRM -> COUNTERS` via `CAN`
  - `RESET_CONFIRM -> COUNTERS` via `CFM` after reset completes
- Any undefined transition request must be ignored and state preserved.

#### MENU command input

- Format:
- `ME:<event>\n`
- Accepted event tokens:
  - `NAV` = navigate
  - `SEL` = select
  - `CAN` = cancel/back
  - `CFM` = confirm
  - `FCL` = flow calibration entrypoint (accepted from `MENU`)
  - `WCL` = wheel calibration entrypoint (accepted from `MENU`)
- Unknown tokens are ignored with no state change.
- Overlong command lines are discarded until newline; no partial token is
  processed after buffer overflow.

#### MENU state output

- Format:
- `MS:<state>\n`
- Output is emitted only when a valid menu event is accepted and processed.
- Valid non-`None` menu events are processed immediately (event-driven), while
  idle state maintenance remains cadence-gated.

### RESET CONFIRM HANDSHAKE (P4 operator interface)

- Reset action is two-step and explicit:
  1. Enter `RESET_CONFIRM` from `COUNTERS` via a reset-select event.
  2. Accept only a `CFM` event to execute reset.
- `CAN` aborts reset and returns to `COUNTERS` with values unchanged.
- On successful `CFM`, both `distance_m` and `area_ha` are set to zero in a single deterministic update cycle.
- Successful reset also restores the persisted calibration profile to defaults,
  resets runtime sensor/controller state, and emits a deterministic reset event
  frame.

#### RESET event output

- Format:
- `RS:COUNTERS_CALIBRATION_RESET\n`
- Emitted exactly once per successful `CFM` reset action.
- Additional deterministic calibration-entrypoint event outputs:
  - `RS:FLOW_CALIBRATION_ENTRYPOINT\n`
  - `RS:WHEEL_CALIBRATION_ENTRYPOINT\n`

### PRESSURE (optional, compile-time gated)

- Enabled only when `ENABLE_PRESSURE_SENSOR=true` in `config.h`; default is `false`.
- Format:
- `PR:<pressure_kpa>\n`
- Description:
- Telemetry-only pressure reading in kPa with 2 decimal places. No control-loop coupling.

## Versioning

- Frozen protocol identifier: `PROTOCOL_V1`
- Documentation alignment date for this protocol contract: `2026-03-23`
