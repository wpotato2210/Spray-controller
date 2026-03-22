# PROTOCOLS.md

Spray Controller Protocols

## Message Formats

| Message | Format | Description |
| --- | --- | --- |
| `FLOW_TOTAL` | `FS:<lpm>\n` | Total measured flow from single FlowSensor |
| `PUMP` | `P:<0-255>\n` | Set global pump PWM duty cycle |
| `SECTION` | `S:<index>:0|1\n` | Report section state (OFF/ON) for each section index |
| `SWITCH` | `SW:0|1\n` | Run/Hold switch state |

## Notes

- Flow messages report total flow only.
- Section messages represent binary ON/OFF state and are emitted once per section each loop.
- No per-section flow message is defined in `PROTOCOL_V1`.

## Versioning

- `PROTOCOL_V1`

## Placeholders

- Full byte-level serialization
- Acknowledgment/error messages
- Error codes / retry logic
