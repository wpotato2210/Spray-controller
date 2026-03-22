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

(km/h) and active spray width (m).

- Active spray width is the sum of ON boom sections (`SECTION_WIDTH_M` per

active section).

- Section changes affect effective width only; flow remains globally controlled

through one pump and one total flow sensor.

## Notes

- Flow calibration required before first operation.
- Safety interlocks should be verified prior to field use.

## Calibration and safety notes

- Runtime calibration reset is available only through the explicit menu path
  `HOME -> MENU -> COUNTERS -> RESET_CONFIRM -> CONFIRM`.
- Fault visibility is provided through status output (`ST`) using deterministic
  `fault_bits` + `fault_text` fields.
- `TARGET_RATE_LPHA` and loop gains (`KP`) remain configurable build constants
  and should be set per sprayer hardware profile before field operation.
