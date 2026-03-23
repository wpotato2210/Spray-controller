# USAGE.md

Spray Controller Usage Instructions

## Operation Steps

1. Power on the system.
1. Set the run/hold switch:

- Run: normal operation
- Hold: stops pump; optional bypass solenoid activated if configured

1. Enable or disable boom sections as needed.
1. Observe LED indicators for each active boom section.

## Operator Interface (P4 deterministic contract)

### Menu states

- `HOME`: Default operator view with fixed-cadence preview payload.
- `MENU`: Navigation root for operator actions.
- `COUNTERS`: Displays distance and sprayed area counters.
- `RESET_CONFIRM`: Explicit confirmation screen for counter reset.

### Required preview payload

The operator preview must include the following fields every publish cycle:

1. `speed_kmh`
1. `flow_lpm`
1. `pump_duty`
1. `active_sections`
1. `distance_m`
1. `area_ha`

### Reset confirmation sequence

1. Navigate `HOME -> MENU -> COUNTERS`.
1. Select reset to enter `RESET_CONFIRM`.
1. Choose one action:
   - `CONFIRM`: reset both distance and area counters to zero.
   - `CANCEL`: return to `COUNTERS` with counters unchanged.
1. On `CONFIRM`, runtime calibration/sensor state is reset via module reset
   hooks and one reset event frame is emitted: `RS:COUNTERS_CALIBRATION_RESET`.

### Operator command/event tokens

- Input command frame: `ME:<event>`
  - `ME:NAV`, `ME:SEL`, `ME:CAN`, `ME:CFM`
- State output frame: `MS:<state>`
  - `HOME`, `MENU`, `COUNTERS`, `RESET_CONFIRM`

### Determinism rules

- Undefined menu transitions are ignored.
- Preview cadence follows the canonical fixed loop interval.
- Counter reset occurs atomically in one deterministic update cycle.

## How Rate Control Works

- The controller maintains target application rate (L/ha) using vehicle speed
  (km/h), active spray width (m), and measured total flow.
- Active spray width is the sum of ON boom sections using the frozen
  `SECTION_WIDTH_M=0.5` value for each active section.
- With three binary boom sections, active width is bounded to `0.0-1.5 m` in
  `0.5 m` increments.
- Section changes affect effective width only; flow remains globally controlled
  through one pump and one total flow sensor.

## Safety and operator-visible runtime behavior

- HOLD forces the controller into the safe output path: pump duty is clamped to
  the configured minimum output and the run state in `ST:` becomes `0`.
- RUN restores closed-loop pump control using current speed, target rate, and
  active width.
- Active faults are surfaced through the deterministic `ST:` fault fields and,
  when the operator interface is connected, remain observable without changing
  menu state semantics.
- Optional pressure telemetry is diagnostic only and does not change pump or
  section outputs.

## Calibration prerequisites

- Flow calibration must be completed before field use and stored calibration is
  used unless the operator enables the documented defaults override workflow.
- Wheel calibration must be completed before relying on distance, area, or
  speed-derived rate control.
- Target rate and control gain are supplied by the frozen configuration assets
  and are not operator-tuned from the documented serial/menu contract.
