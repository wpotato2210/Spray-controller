# PROTOCOLS.md

Spray Controller Protocols

## Message Formats

| Message | Format | Description |
| --- | --- | --- |
| `FLOW_TOTAL` | `FT:<L/min>\n` | Total measured flow from single FlowSensor |
| `PUMP` | `P:<0-255>\n` | Set global pump PWM duty cycle |
| `SECTION` | `S:<index>:<0\|1>\n` | Set/report section state (OFF/ON) |
| `SECTIONS` | `SS:<bitmask>\n` | Set/report all section states as bitmap |
| `SWITCH` | `SW:<0\|1>\n` | Run/Hold switch state |
Message	Format	Description
FLOW_TOTAL	FS:<lpm>\n	Total measured flow from single FlowSensor
PUMP	P:<0-255>\n	Set global pump PWM duty cycle
SECTION	S:<index>:<0|1>\n	Set/report individual boom section state (OFF/ON)
SECTIONS	SS:<bitmask>\n	Set/report all boom section states as ON/OFF bitmap
SWITCH	SW:<0|1>\n	Run/Hold switch state

## Notes

- Flow messages report total flow only.
- Section messages represent binary ON/OFF state.
- No per-section flow message is defined in `PROTOCOL_V1`.

## Versioning

- `PROTOCOL_V1`

## Placeholders

- Full byte-level serialization
- Acknowledgment/error messages
- Error codes / retry logic
