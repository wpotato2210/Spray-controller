# CALIBRATION.md

Calibration Procedures

## Flow Sensor Calibration

1. Reset calibration override to defaults or clear prior calibrated flow
   constant.
2. Prime the plumbing with clean water and run the pump at a fixed PWM
   duty until flow stabilizes.
3. Collect pulses and measured liters over a bounded sample window
   (minimum 10 L recommended).
4. Compute `flow_pulses_per_liter = total_pulses / measured_liters` and
   reject zero or negative results.
5. Accept the calibration only when the recomputed factor is within a
   documented sanity window and repeated runs agree within 5%.

- Sensor model: single YF-S201C total-flow sensor.
- Baseline default: `FLOW_PULSES_PER_LITER = 450.0`.
- Sanity window: accept only `300.0 <= flow_pulses_per_liter <= 700.0`
  for the YF-S201C baseline build.
- Persistence: accepted flow calibration is stored in non-volatile memory
  with a validity marker and checksum; invalid or blank storage reloads the
  `config.h` default `FLOW_PULSES_PER_LITER` on boot.

## Wheel Calibration

1. Reset any prior wheel-distance override to the default wheel geometry
   constants.
2. Mark a straight test course of known length (minimum 50 m recommended)
   and set tire pressure to the normal operating value before moving.
3. Drive the wheel sensor over the full measured course while recording
   total wheel pulses and reject runs with zero pulses or wheel slip.
4. Compute `wheel_distance_per_pulse_m = measured_distance_m /
   total_wheel_pulses` and `wheel_pulses_per_100m = (total_wheel_pulses /
   measured_distance_m) * 100.0`.
5. Accept the calibration only when `0.02 <= wheel_distance_per_pulse_m <=
   0.5` and repeated runs agree within 5%.

- Sensor model: single wheel pickup using `WHEEL_CIRCUMFERENCE_M` and
  `WHEEL_PULSES_PER_REV`.
- Baseline defaults: `WHEEL_CIRCUMFERENCE_M = 2.0`,
  `WHEEL_PULSES_PER_REV = 20.0`.
- Derived default: `wheel_distance_per_pulse_m = 0.1`.
- Persistence: accepted wheel calibration is stored in non-volatile memory
  with a validity marker and checksum; invalid or blank storage reloads the
  `config.h` defaults for `WHEEL_CIRCUMFERENCE_M` and
  `WHEEL_PULSES_PER_REV` on boot.

## Pump Calibration

- PWM vs total-flow curve: [placeholder]

## Controller Calibration

- `TARGET_RATE_LPHA`: [placeholder]
- `SECTION_WIDTH_M`: [placeholder]
- `KP`: [placeholder]
- `MIN_SPEED_KMH`: [placeholder]

## Placeholders

- Operator override flag: persisted `use_defaults_override` selector that
  forces runtime flow and wheel conversions to use `config.h` defaults while
  retaining the last accepted calibrated constants in storage.
- Operator override workflow:
  1. Set `use_defaults_override = true` before field validation when the
     operator wants to ignore stored calibrated constants.
  2. Verify runtime flow and wheel math resolve from `FLOW_PULSES_PER_LITER`,
     `WHEEL_CIRCUMFERENCE_M`, and `WHEEL_PULSES_PER_REV`.
  3. Set `use_defaults_override = false` to resume the persisted calibrated
     profile without recomputing or re-entering the accepted constants.
- Field calibration method
- Acceptance tolerances beyond the flow sanity window
