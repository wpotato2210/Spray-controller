# CONFIGURATION.md

Spray Controller Configuration Parameters

| Section | Key | Default | Description |
| --- | --- | --- | --- |
| `application` | `target_rate_lpha` | 100.0 | Target application rate (L/ha). |
| `application` | `section_width_m` | 0.5 | Width per boom section (m). |
| `control.gains` | `kp` | 2.0 | Proportional gain for global flow control. |
| `control.gains` | `ki` | 0.2 | Integral gain for global flow control. |
| `control` | `pwm_min` | 0 | Minimum pump duty cycle (PWM counts). |
| `control` | `pwm_max` | 255 | Maximum pump duty cycle (PWM counts). |
| `control.integral_limits` | `min` | -80.0 | Lower clamp for integral term. |
| `control.integral_limits` | `max` | 80.0 | Upper clamp for integral term. |
| `control` | `slew_limit_pwm_per_cycle` | 12.0 | Max PWM step per control loop to reduce command jumps. |
| `limits` | `min_speed_kmh` | 0.5 | Minimum valid speed for rate control (km/h). |
| `limits` | `max_flow_lpm` | 100.0 | Maximum allowed flow (L/min). |
| `limits` | `flow_fault_lpm_threshold` | 0.03 | Measured-flow threshold for no-flow fault detection (L/min). |
| `limits` | `min_target_flow_for_fault_lpm` | 0.2 | Minimum target flow before applying no-flow fault logic (L/min). |
| `timing` | `loop_interval_ms` | 50 | Main control loop interval (ms). |
| `timing` | `telemetry_interval_ms` | 200 | Telemetry output interval (ms). |
| `timing` | `sensor_fault_timeout_ms` | 1200 | Timeout for stale sensor updates before fault handling (ms). |

## Placeholders

- Calibration-derived defaults for `target_rate_lpha`, `section_width_m`, and
  control gains.
- Optional parameters for actuator ramp-up and delayed section sequencing.
