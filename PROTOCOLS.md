PROTOCOLS.md

Spray Controller Protocols

Message Formats

Message	Format	Description
FLOW	FS::<L/min>\n	Flow reading per section
PUMP	P:\n	Set pump PWM duty cycle
SECTION	S::\n	Section open/closed
SWITCH	SW:\n	Run/hold switch state

Versioning
	•	PROTOCOL_V1

Placeholders
	•	Full byte-level serialization
	•	Acknowledgment/error messages
	•	Error codes / retry logic