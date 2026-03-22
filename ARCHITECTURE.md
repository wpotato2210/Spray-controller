# ARCHITECTURE

Spray Controller Architecture

## Layer Separation (SEE / THINK / DO)

```text
+----------------------+      +----------------------+      +----------------------+
|      SEE Layer       | ---> |     THINK Layer      | ---> |       DO Layer       |
|----------------------|      |----------------------|      |----------------------|
| FlowSensor (total)   |      | SystemState          |      | PumpControl (PWM)    |
| WheelSensor          |      | SectionManager       |      | BoomSection[]        |
| RunHoldSwitch        |      | FlowController       |      |                      |
+----------------------+      +----------------------+      +----------------------+
```

## Global Flow Control Model

- Single FlowSensor measures total system flow.
- Multiple BoomSection outputs are binary ON/OFF.
- FlowController computes one pump command for active boom width.
- SectionManager tracks active sections and effective spray width.

## Frozen Module Interfaces

### FlowSensor

- Inputs: pulse input from sensor
- Outputs: totalFlowRate (L/min)
- Methods: readFlow(), reset()
- Notes: single YF-S201C total-flow sensor

### WheelSensor

- Inputs: digital pulse input
- Outputs: vehicleSpeed (km/h)
- Methods: readSpeed(), reset()
- Notes: used for rate normalization

### RunHoldSwitch

- Inputs: digital input (bool)
- Outputs: runEnabled (bool)
- Methods: readRunHold()
- Notes: run/hold operator control

### SystemState

- Inputs: run/hold, faults, section states
- Outputs: systemMode
- Methods: update(), getMode()
- Notes: tracks run/hold/fault states

### SectionManager

- Inputs: section switch states (bool[])
- Outputs: activeSections, activeWidth (m)
- Methods: setSection(), getActiveWidth()
- Notes: sections are ON/OFF only

### FlowController

- Inputs: targetRate, speed, activeWidth, totalFlowRate
- Outputs: pumpCommand (PWM 0-255)
- Methods: computePumpDuty(), stop()
- Notes: global closed-loop flow control

### PumpControl

- Inputs: pumpCommand (PWM 0-255)
- Outputs: PWM signal (0-255)
- Methods: startPWM(), stopPWM(), setDutyCycle()
- Notes: single global pump output

### BoomSection

- Inputs: sectionCommand (bool)
- Outputs: sectionState (bool)
- Methods: open(), close(), status()
- Notes: binary valve state only

## Placeholders

- Module update intervals for SEE/THINK/DO timing
- Error and alert outputs for sensor failures
- UML or modular connection diagram
- Versioning of frozen interfaces
- Wiring diagram references for each boom section
