# DIAGRAMS.md

System Diagrams

## Architecture (SEE / THINK / DO with operator interface)

```text
[Sensors]
  - FlowSensor::readFlow()         (L/min)
  - WheelSpeedSensor::readSpeedKmh() (km/h)
  - Run/Hold switch + section inputs
              |
              v
[THINK: deterministic loop @ LOOP_INTERVAL_MS]
  - RateController::computeDuty()
  - CoverageAccumulator::update()
  - OperatorMenuStateMachine::update()
  - Fault evaluation + telemetry gating
              |
              +--------------------+
              |                    |
              v                    v
[DO: actuators]               [Operator telemetry I/O]
  - Pump PWM output             - ST:<...> status frame
  - Bypass solenoid output      - PV:<speed,flow,duty,sections,distance,area>
  - Section driver outputs      - MS:<state>
                                - RS:COUNTERS_CALIBRATION_RESET
```

## Operator menu state machine (P4)

```text
Input command (serial): ME:<event>
Accepted events: NAV, SEL, CAN, CFM

HOME --NAV--> MENU --SEL--> COUNTERS --SEL--> RESET_CONFIRM --CFM--> COUNTERS
  ^              |             ^                  |                     |
  |              +--CAN--------+                  +--CAN---------------+
  +------------------------------(MENU CAN -> HOME)

Undefined event/state pairs: ignored (state preserved).
```

## Operator preview + reset dependencies

```text
[WheelSpeedSensor] --speed_kmh--+
[FlowSensor] ------flow_lpm------+----> publishPreview() -> PV frame
[RateController] --pump_duty-----+
[Section mask] ---active_sections+
[CoverageAccumulator] --distance_m, area_ha--+

[OperatorMenuStateMachine consumeResetConfirmed()]
        |
        +--> CoverageAccumulator::reset()
        +--> FlowSensor::reset()
        +--> WheelSpeedSensor::reset()
        +--> RateController::reset()
        +--> publishResetEvent() -> RS:COUNTERS_CALIBRATION_RESET
```

## Wiring (logical)

```text
MCU GPIO/PWM
  -> Pump driver (PWM)
  -> Bypass solenoid driver (digital)
  -> Section outputs S0/S1/S2 (digital)

Inputs
  <- Flow pulse input
  <- Wheel pulse input
  <- Run/Hold input
  <- Section toggle inputs

UART
  <-> Host/operator console (ME commands, ST/PV/MS/RS frames)
```
