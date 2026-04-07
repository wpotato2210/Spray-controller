# Frozen Interface Checklist

Scope audited: `include/interfaces.h`, `include/arduino_adapters.h`,
`Spray-controller.ino`, and all `src/*.cpp` files.

## Compile/include path note

- `Arduino.h` is provided by the Arduino core/toolchain and is not stored in
  this repository.
- Project-local headers are included as `config.h`, `interfaces.h`, `pins.h`,
  and `protocol.h`; non-Arduino toolchains must compile with `-Iinclude`.
- GitHub Actions compile checks run through `ci/compile-sketch.sh`, which adds
  `-I$GITHUB_WORKSPACE/include` for both C and C++ compilation.

## Frozen interface to implementation checklist

| Frozen interface | Declared in | Current implementation owner(s) | Coverage note |
| --- | --- | --- | --- |
| `DigitalInputAdapter` | `include/interfaces.h` | `ArduinoActiveLowInput` in `include/arduino_adapters.h` / `src/arduino_adapters.cpp` | Implements `beginPullup()` and `isActive()` for active-low switch inputs. |
| `DigitalOutputAdapter` | `include/interfaces.h` | `ArduinoActiveHighOutput` in `include/arduino_adapters.h` / `src/arduino_adapters.cpp` | Implements `beginLow()` and `write()` for pump/indicator/section outputs. |
| `AnalogInputAdapter` | `include/interfaces.h` | `ArduinoAnalogInput` in `include/arduino_adapters.h` / `src/arduino_adapters.cpp` | Implements `begin()` and `readRaw()` for optional pressure telemetry. |
| `PwmOutputAdapter` | `include/interfaces.h` | `ArduinoPwmOutput` in `include/arduino_adapters.h` / `src/arduino_adapters.cpp` | Implements `begin()` and `write()` for pump PWM output. |
| `PulseCounterAdapter` | `include/interfaces.h` | `ArduinoInterruptPulseCounter` in `include/arduino_adapters.h` / `src/arduino_adapters.cpp` | Implements `begin()`, `readCount()`, `readLastPulseMs()`, and `reset()` for flow/wheel pulse capture. |
| `SectionHardwareAdapter` | `include/interfaces.h` | `ArduinoSectionHardwareAdapter` in `include/arduino_adapters.h` / `src/arduino_adapters.cpp` | Implements `begin()`, `readSwitch()`, and `writeSection()` for all section I/O. |
| `FlowSensor` | `include/interfaces.h` | `src/flow_sensor.cpp` | Implements `begin()`, `readFlow()`, `reset()`, `isStaleFaultActive()`, and `isConfigFaultActive()`. |
| `WheelSensor` | `include/interfaces.h` | `src/wheel_sensor.cpp` | Implements `begin()`, `readSpeed()`, `reset()`, `isTimeoutFaultActive()`, and `isConfigFaultActive()`. |
| `PressureSensor` | `include/interfaces.h` | `src/pressure_sensor.cpp` | Implements `begin()`, `readPressure()`, `reset()`, and `isConfigFaultActive()`; used only when `ENABLE_PRESSURE_SENSOR` is true. |
| `RunHoldSwitch` | `include/interfaces.h` | `src/run_hold_switch.cpp` | Implements `begin()` and `readRunHold()` on top of `DigitalInputAdapter`. |
| `SectionManager` | `include/interfaces.h` | `src/section_manager.cpp` | Implements section state storage plus `getActiveCount()` and `getActiveWidth()`. |
| `FlowController` | `include/interfaces.h` | `src/flow_controller.cpp` | Implements `computePumpDuty()` and `stop()` for the single global pump model. |
| `PumpControl` | `include/interfaces.h` | `src/pump_control.cpp` | Implements `begin()`, `startPWM()`, and `setDutyCycle()` on top of `PwmOutputAdapter`. |
| `CoverageAccumulator` | `include/interfaces.h` | `src/coverage_accumulator.cpp` | Implements `update()`, `reset()`, `getDistanceMeters()`, and `getAreaHectares()`. |

## Runtime ownership link from checklist to loop

- `setup()` in `Spray-controller.ino` initializes the concrete adapter-backed
  objects before loop execution.
- `loop()` in `Spray-controller.ino` uses the frozen classes in a fixed order:
  section read -> sensor read -> active-width/coverage compute -> flow control
  compute -> pump/section writes -> telemetry publish.
- No interface declared in `include/interfaces.h` is currently orphaned; every
  declaration has one repository-local implementation owner or concrete derived
  class.
