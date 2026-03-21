ARCHITECTURE.md

Spray Controller Architecture

Layer Separation (See / Think / Do)

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

Global Flow Control Model
	•	Single FlowSensor measures total system flow.
	•	Multiple BoomSection outputs are binary (ON/OFF).
	•	FlowController is global and computes one pump command for the active boom width.
	•	SectionManager tracks active sections and exposes effective spray width.

Frozen Module Interfaces

Module	Inputs	Outputs	Methods / Actions	Notes / Units
FlowSensor	Pulse input from sensor	totalFlowRate (L/min)	readFlow() -> L/min, reset()	Single YF-S201C sensor for total flow
WheelSensor	Digital pulse input	vehicleSpeed (km/h)	readSpeed() -> km/h, reset()	Used for rate normalization
RunHoldSwitch	Digital input (bool)	runEnabled (bool)	readRunHold() -> bool	Run/Hold operator control
SystemState	RunHold + faults + section states	systemMode	update(), getMode()	Tracks run/hold/fault states
SectionManager	Section switch states (bool[])	activeSections (bool[]), activeWidth (m)	setSection(index, state), getActiveWidth()	Sections are ON/OFF only
FlowController	targetRate (L/ha), speed (km/h), activeWidth (m), totalFlowRate (L/min)	pumpCommand (PWM 0–255)	computePumpDuty(), stop()	Global closed-loop flow control
PumpControl	pumpCommand (PWM 0–255)	PWM signal (0–255)	startPWM(), stopPWM(), setDutyCycle(duty)	Single global pump output
BoomSection	sectionCommand (bool)	sectionState (bool)	open(), close(), status() -> bool	Binary valve state only

Placeholders
	•	Module update intervals (SEE/THINK/DO loop timing)
	•	Error/alert outputs for sensor failures
	•	UML / modular connection diagram (optional)
	•	Versioning of frozen interfaces
	•	Wiring diagram references for each boom section
