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

## Placeholders

- Calibration-derived defaults for `TARGET_RATE_LPHA`, `SECTION_WIDTH_M`, and
  `KP`
- Adjustable safety thresholds
- Optional parameters for PWM ramp-up or delays
