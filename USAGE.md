# USAGE.md

Spray Controller Usage Instructions

## Operation Steps

1. Power on the system and wait for serial startup at `115200` baud.
2. Set the run/hold switch:
   - Run: closed switch enables closed-loop pump control.
   - Hold: pump duty is forced to `0` and the flow controller is stopped.
3. Enable or disable boom sections as needed using the three section toggle inputs.
4. Observe the section indicator/output state for each active boom section.
5. Monitor `PV:` preview telemetry and `ST:` status telemetry during operation.

## Operator Interface (P4 deterministic contract)

### Menu states

- `HOME`: Default operator view with fixed-cadence preview payload.
- `MENU`: Navigation root for operator actions.
- `COUNTERS`: Displays distance and sprayed area counters.
- `RESET_CONFIRM`: Explicit confirmation screen for counter reset.

### Required preview payload

The operator preview publishes the following fields every preview cycle in this exact order:

1. `speed_kmh`
2. `flow_lpm`
3. `pump_duty`
4. `active_sections`
5. `distance_m`
6. `area_ha`

### Reset confirmation sequence

1. Navigate `HOME -> MENU -> COUNTERS`.
2. Select reset to enter `RESET_CONFIRM`.
3. Choose one action:
   - `CFM`: reset both distance and area counters to zero, restore persisted calibration to defaults, and reset runtime controller/sensor state.
   - `CAN`: return to `COUNTERS` with counters unchanged.
4. On `CFM`, one reset event frame is emitted: `RS:COUNTERS_CALIBRATION_RESET`.

### Operator command/event tokens

- Input command frame: `ME:<event>`
  - `ME:NAV`, `ME:SEL`, `ME:CAN`, `ME:CFM`, `ME:FCL`, `ME:WCL`
  - Overlong command lines are dropped until newline (safety against partial-frame parse).
- State output frame: `MS:<state>`
  - `HOME`, `MENU`, `COUNTERS`, `RESET_CONFIRM`
- Event output frame: `RS:<event>`
  - `COUNTERS_CALIBRATION_RESET`, `FLOW_CALIBRATION_ENTRYPOINT`,
    `WHEEL_CALIBRATION_ENTRYPOINT`

### Determinism rules

- Undefined menu transitions are ignored.
- Valid menu events are handled immediately when received; periodic cadence applies to idle maintenance ticks.
- Preview cadence follows `PREVIEW_INTERVAL_MS`; the default publish interval is `50 ms`.
- Counter reset occurs atomically in one deterministic update cycle.
- Telemetry emission is gated by serial TX capacity; the controller never backfills skipped frames.

## How Rate Control Works

- The controller maintains target application rate using vehicle speed (km/h), active spray width (m), and target rate `TARGET_RATE_LPHA = 100.0`.
- Active spray width is the sum of enabled boom sections at `SECTION_WIDTH_M = 0.5` meters per section.
- Pump duty is bounded to `0-255` and updated by the PI controller using `KP = 2.0` and `KI = 0.2`.
- Below `MIN_SPEED_KMH = 0.5`, requested target flow collapses to zero and the controller ramps duty toward the safe fallback path.
- Section changes affect effective width only; flow remains globally controlled through one pump and one total-flow sensor.

## Operating Notes

- Flow and wheel calibration should be completed before first field use.
- Pressure telemetry is optional and disabled by default (`ENABLE_PRESSURE_SENSOR=false`).
- Verify run/hold, section switches, and sensor wiring before enabling spray output.
- Review `ST:` fault bits; `fault_text=FAULT` indicates at least one sensor/configuration fault is active.
