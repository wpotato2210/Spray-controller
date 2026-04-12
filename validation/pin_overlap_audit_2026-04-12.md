# Pin overlap audit (PIN_BOOM_*vs PIN_SECTION_SW_*)

## Scope

- Symbols: `PIN_BOOM_1..3`, `PIN_SECTION_SW_1..3`
- Targets: UNO, NANO, MEGA2560 pin maps
- Analysis date: 2026-04-12

## Definitions

### UNO

- `PIN_BOOM_1 = 12`, `PIN_BOOM_2 = 7`, `PIN_BOOM_3 = A1`
- `PIN_SECTION_SW_1 = A4`, `PIN_SECTION_SW_2 = A5`, `PIN_SECTION_SW_3 = 4`
- Mapped by section descriptor tuples `{id, output_pin, switch_pin}`.

### NANO

- `PIN_BOOM_1 = 12`, `PIN_BOOM_2 = 7`, `PIN_BOOM_3 = A1`
- `PIN_SECTION_SW_1 = A4`, `PIN_SECTION_SW_2 = A5`, `PIN_SECTION_SW_3 = 4`
- Mapped by section descriptor tuples `{id, output_pin, switch_pin}`.

### MEGA2560

- `PIN_BOOM_1 = 22`, `PIN_BOOM_2 = 23`, `PIN_BOOM_3 = 24`
- `PIN_SECTION_SW_1 = 27`, `PIN_SECTION_SW_2 = 28`, `PIN_SECTION_SW_3 = 29`
- Mapped by section descriptor tuples `{id, output_pin, switch_pin}`.

## Usage classification

- Direct symbol references are only in board pin headers and a validation script/docs.
- Runtime behavior is driven through `SectionDescriptor.output_pin` and `SectionDescriptor.switch_pin`.

### Runtime direction

- `switch_pin` path:
  - Assigned to `ArduinoActiveLowInput` in `ArduinoSectionHardwareAdapter` constructor.
  - Initialized with `pinMode(INPUT_PULLUP)`.
  - Read with `digitalRead(...) == LOW`.
- `output_pin` path:
  - Assigned to `ArduinoActiveHighOutput` in `ArduinoSectionHardwareAdapter` constructor.
  - Initialized with `pinMode(OUTPUT)` and low default.
  - Driven with `digitalWrite(HIGH/LOW)`.

## Signal flow

- Section 1: `PIN_SECTION_SW_1 -> readSwitch(0) -> SectionManager.setSection(0, ...) -> writeSection(0, state) -> PIN_BOOM_1`
- Section 2: `PIN_SECTION_SW_2 -> readSwitch(1) -> SectionManager.setSection(1, ...) -> writeSection(1, state) -> PIN_BOOM_2`
- Section 3: `PIN_SECTION_SW_3 -> readSwitch(2) -> SectionManager.setSection(2, ...) -> writeSection(2, state) -> PIN_BOOM_3`

## Conclusion

- Functional duplication: NO
- Partial overlap: NO (logical pairing only; no interchangeable use in code)
- Intentional distinction: YES (`switch_pin` input layer vs `output_pin` actuator layer)

## Architecture alignment (SEE -> THINK -> DO)

- SEE: `readSwitch(...)` reads active-low operator switch inputs.
- THINK: `SectionManager` stores logical section state.
- DO: `writeSection(...)` drives boom output pins.
- Layer violation detected: NO.
