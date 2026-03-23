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
- Persistence hand-off: until P6 storage is implemented, record the
  accepted factor in `config.h` as `FLOW_PULSES_PER_LITER` before the
  next run.

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
- Persistence hand-off: until P6 storage is implemented, update `config.h`
  so `WHEEL_CIRCUMFERENCE_M / WHEEL_PULSES_PER_REV` matches the accepted
  wheel distance per pulse before the next run.

## Pump Calibration

- PWM vs total-flow curve: [placeholder]

## Controller Calibration

- `TARGET_RATE_LPHA`: [placeholder]
- `SECTION_WIDTH_M`: [placeholder]
- `KP`: [placeholder]
- `MIN_SPEED_KMH`: [placeholder]

## Placeholders

- Persistent calibration storage with validity marker/checksum
- Operator override workflow (defaults vs calibrated constants)
- Field calibration method
- Acceptance tolerances beyond the flow sanity window
