# CONFIGURATION

Spray Controller Configuration Parameters

## Parameters

- PWM_MAX: 255, maximum pump duty cycle (PWM counts)
- TARGET_RATE_LPHA: TBD, target application rate (L/ha)
- SECTION_WIDTH_M: TBD, width per boom section (m)
- KP: TBD, proportional gain for global FlowController
- MIN_SPEED_KMH: TBD, minimum valid speed for rate control (km/h)
- SECTION_TIMEOUT: 500 ms, section switch debounce time
- LOOP_INTERVAL_MS: 50 ms, main control loop interval
- FLOW_PULSES_PER_LITER: 450.0, flow sensor calibration
- WHEEL_CIRCUMFERENCE_M: 2.0, wheel circumference for speed estimation
- WHEEL_PULSES_PER_REV: 20.0, wheel pulse calibration

## Placeholders

- Calibration defaults for TARGET_RATE_LPHA, SECTION_WIDTH_M, and KP
- Adjustable safety thresholds
- Optional PWM ramp-up or delay parameters
