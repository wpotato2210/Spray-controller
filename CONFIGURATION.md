# CONFIGURATION.md

Spray Controller Configuration Parameters

| Parameter | Default | Description |
| --- | --- | --- |
| `PWM_MAX` | 255 | Maximum pump duty cycle (PWM counts) |
| `TARGET_RATE_LPHA` | [TBD] | Target application rate (L/ha) |
| `SECTION_WIDTH_M` | [TBD] | Width per boom section (m) |
| `KP` | [TBD] | Proportional gain for global `FlowController` |
| `MIN_SPEED_KMH` | [TBD] | Minimum valid speed for rate control (km/h) |
| `SECTION_TIMEOUT` | 500 ms | Section switch debounce time (ms) |
| `LOOP_INTERVAL_MS` | 50 ms | Main control loop interval (ms) |
| `FLOW_PULSES_PER_LITER` | 450.0 | Flow sensor calibration (pulses per liter) |
| `WHEEL_CIRCUMFERENCE_M` | 2.0 | Wheel circumference for speed estimation (m) |
| `WHEEL_PULSES_PER_REV` | 20.0 | Wheel pulse calib. (pulses/rev) |

## Placeholders

- Calibration-derived defaults for `TARGET_RATE_LPHA`, `SECTION_WIDTH_M`, and
  `KP`
- Adjustable safety thresholds
- Optional parameters for PWM ramp-up or delays
