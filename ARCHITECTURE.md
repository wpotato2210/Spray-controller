# ARCHITECTURE.md

Spray Controller Architecture

## Layer Separation (See / Think / Do)

```text
                 +----------------------+
                 |      SEE Layer       |
                 |----------------------|
                 | FlowSensor (total)   |
                 | WheelSensor          |
                 | RunHoldSwitch        |
                 +----------+-----------+
                            |
                            v
                 +----------------------+
                 |     THINK Layer      |
                 |----------------------|
                 | SystemState          |
                 | SectionManager       |
                 | FlowController       |
                 +----------+-----------+
                            |
                            v
                 +----------------------+
                 |       DO Layer       |
                 |----------------------|
                 | PumpControl (PWM)    |
                 | BoomSection[]        |
                 +----------------------+
```

## Global Flow Control Model

- Single `FlowSensor` measures total system flow.
- Multiple `BoomSection` outputs are binary (ON/OFF).
- `FlowController` is global and computes one pump command for the active boom
  width.
- `SectionManager` tracks active sections and exposes effective spray width.

## Frozen Module Interfaces

| Module | Inputs | Outputs | Methods / Actions | Notes / Units |
| --- | --- | --- | --- | --- |
| FlowSensor | pulse | total_flow_lpm | `readLpm()` | single global flow |
| WheelSensor | pulse | speed_kmh | `readSpeed()` | used for rate control |
| RunHoldSwitch | digital in | run_state | `isRun()` | hold may trigger bypass |
| SystemState | sensors + switches | state struct | `update()` | aggregates runtime state |
| SectionManager | section switches | active_sections, width_m | `updateSections()` | ON/OFF only |
| FlowController | target_rate, speed, width, total_flow | pump_pwm | `computePWM()` | global control loop |
| PumpControl | pwm command | hardware signal | `setPWM()` | `0..255` |
| BoomSection[i] | section cmd | valve state | `setOn(bool)` | binary outputs |
