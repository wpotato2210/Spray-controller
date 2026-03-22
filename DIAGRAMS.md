# DIAGRAMS.md

System Diagrams

## SEE / THINK / DO (runtime data path)

```text
SEE (inputs)                                THINK (compute)                            DO (outputs)
┌─────────────────────────────┐             ┌───────────────────────────────────────┐   ┌──────────────────────────────────┐
│ FlowSensor::readFlow()      │──flow_lpm──>│ FlowController::computePumpDuty(...)  │──>│ PumpControl::setDutyCycle(duty)  │
│ WheelSensor::readSpeed()    │─speed_kmh──>│ CoverageAccumulator::update(...)       │──>│ ST/PV telemetry publish           │
│ RunHoldSwitch::readRunHold()│──run_en────>│ OperatorMenuStateMachine::update(...)  │──>│ MS state frame / RS reset frame   │
│ Section switches (GPIO)     │─section_on─>│ SectionManager active width/count      │──>│ Boom outputs + section LED        │
└─────────────────────────────┘             └───────────────────────────────────────┘   └──────────────────────────────────┘
```

Dependencies:

- `Spray-controller.ino` orchestrates loop cadence and module ordering.
- `operator_menu` consumes `ME:*` commands and emits `MS:*` transitions.
- `coverage_accumulator` owns `distance_m` + `area_ha` preview counters.

## Operator menu state flow

```text
ME:NAV                 ME:SEL                     ME:SEL
HOME ───────────────▶ MENU ────────────────────▶ COUNTERS ───────────────▶ RESET_CONFIRM
  ▲                    │  ▲                         ▲   │                    │       │
  │                    │  │                         │   │                    │       │
  └────── ME:CAN ◀─────┘  └────────── ME:CAN ──────┘   └────── ME:CAN ─────┘       │
                                                                                     │
                                          ME:CFM (exec reset + RS event) ────────────┘
                                          transition target after CFM: COUNTERS
```

I/O contract:

- Input command frame: `ME:<event>\n` where event in `{NAV,SEL,CAN,CFM}`.
- State output frame: `MS:<state>\n` where state in
  `{HOME,MENU,COUNTERS,RESET_CONFIRM}`.
- Reset event output frame: `RS:COUNTERS_CALIBRATION_RESET\n`
  (exactly once on successful confirm).

## Wiring (logical)

```text
[Section Switches] ──GPIO in──▶ SectionManager ──state──▶ [Boom MOSFET Outputs]
[Run/Hold Switch ] ──GPIO in──▶ RunHoldSwitch  ──run_en─▶ FlowController gate
[Flow Sensor     ] ──INT pin──▶ FlowSensor
[Wheel Sensor    ] ──INT pin──▶ WheelSensor
[Pump PWM pin    ] ◀────────── PumpControl
[UART Serial     ] ◀────────── ST / PV / MS / RS / PR(optional)
```

Primary protocol dependencies:

- `ST:` status (`flow,pump_duty,run,section_mask,fault_bits,fault_text`)
- `PV:` preview (`speed,flow,duty,active_sections,distance,area`)
- `MS:` operator menu state
- `RS:` reset confirmation event
- `PR:` optional pressure telemetry when compile-time enabled
