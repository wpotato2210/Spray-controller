# PROTOCOLS

Spray Controller Protocols

## Message Formats

| Message | Format | Description |
| --- | --- | --- |
| FLOW_TOTAL | FS:value\\n | Total measured flow from single FlowSensor |
| PUMP | P:0-255\\n | Set global pump PWM duty cycle |
| SECTION | S:index:0-or-1\\n | Set or report individual boom section state |
| SECTIONS | SS:bitmask\\n | Set or report all boom section states bitmap |
| SWITCH | SW:0-or-1\\n | Run/Hold switch state |

## Notes

- Flow messages report total flow only.
- Section messages represent binary ON/OFF state.
- No per-section flow message exists in PROTOCOL_V1.

## Versioning

- PROTOCOL_V1

## Placeholders

- Full byte-level serialization
- Acknowledgment and error messages
- Error codes and retry logic
