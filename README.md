README.md

Spray Controller

Overview

Automated boom sprayer controller for precision agricultural application.

Components
	•	MCU: Arduino Nano (5V logic)
	•	Pump: 12V PWM-controlled
	•	Boom sections with LEDs
	•	Flow sensors (YF-S201C)
	•	Switches: individual sections + run/hold (with optional bypass solenoid)

Quickstart
	1.	Connect MCU, sensors, and pumps according to HARDWARE.md
	2.	Upload firmware (see INSTALL.md)
	3.	Toggle run/hold switch to operate

Dependencies
	•	Arduino IDE
	•	Libraries: Servo, Wire
	•	Flow sensors: YF-S201C

Placeholders
	•	High-level project diagram
	•	Supported MCU alternatives beyond Nano (optional)

⸻

ARCHITECTURE.md

Spray Controller Architecture

Layer Separation (See / Think / Do)

                 +----------------------+
                 |      SEE Layer       |
                 |----------------------|
                 | Wheel Sensor         |
                 | Flow Sensors         |
                 | Boom Section Switches|
                 +----------+-----------+
                            |
                            v
                 +----------------------+
                 |     THINK Layer      |
                 |----------------------|
                 | EventManager         |
                 | Flow Monitoring      |
                 | Run/Hold Logic       |
                 | PWM Calculation      |
                 +----------+-----------+
                            |
                            v
                 +----------------------+
                 |       DO Layer       |
                 |----------------------|
                 | PumpControl (PWM)    |
                 | BoomSection Valves    |
                 | Section LEDs         |
                 | Optional Bypass Sol. |
                 +----------------------+

Frozen Module Interfaces

Module	Inputs	Outputs	Methods / Actions	Notes / Units
PumpControl	targetFlow (L/min)	PWM signal (0–255)	startPWM(speed), stopPWM(), setDutyCycle(duty)	Maintains target flow per section
FlowSensor	Pulse input from sensor	flowRate (L/min)	readFlow() -> L/min, reset()	YF-S201C sensor
BoomSection	Section switch state (bool)	sectionState (bool)	open(), close(), status() -> bool	LED mirrors section state
RunHoldSwitch	Digital input (bool)	systemState (bool)	readRunHold() -> bool	Optional bypass solenoid
WheelSensor	Digital pulse input	wheelSpeed (L/min)	readSpeed(), reset()	Optional for travel-speed calc
EventManager	Module events / states	Triggers to modules	handleEvent(eventID)	Coordinates See/Think/Do layers

Placeholders
	•	Module update intervals (SEE/THINK/DO loop timing)
	•	Error/alert outputs for sensor failures
	•	UML / modular connection diagram (optional)
	•	Versioning of frozen interfaces
	•	Wiring diagram references for each boom section

⸻

PROTOCOLS.md

Spray Controller Protocols

Message	Format	Description
FLOW	FS::<L/min>\n	Flow reading per section
PUMP	P:\n	Set pump PWM duty cycle
SECTION	S::\n	Section open/closed
SWITCH	SW:\n	Run/hold switch state

Versioning: PROTOCOL_V1

Placeholders:
	•	Full byte-level serialization
	•	Acknowledgment/error messages

⸻

HARDWARE.md

Spray Controller Hardware

Pinout

Pin	Component	Notes
2	Wheel Sensor	Digital input
3	Flow Sensor	Pulse input
11	Pump PWM	PWM output
12	Boom Section 1	Digital output
13	Boom Section 2	Digital output
A1	Boom Section 3	Digital output
A2	Section 1 LED	Indicator

Switches
	•	Individual section on/off
	•	Run/Hold switch with optional bypass solenoid

Power
	•	Pump: 12V supply
	•	MCU: 5V logic

ASCII Wiring Diagram

MCU Nano (5V logic)
+----------------------+
| 2  -> Wheel Sensor   |----> Pulse input
| 3  -> Flow Sensor    |----> Pulse input (YF-S201C)
| 11 -> Pump PWM       |----> 12V Pump via N-MOSFET / Relay
| 12 -> Boom 1         |----> Solenoid 1
| 13 -> Boom 2         |----> Solenoid 2
| A1 -> Boom 3         |----> Solenoid 3
| A2 -> LED Boom 1     |----> Indicator LED
| GND -----------------|----> Common GND
| 5V -----------------|----> MCU logic supply
+----------------------+

Run/Hold Switch:
+----------------------+
| Input -> Digital Pin  |----> EventManager read
| Optional -> Bypass Solenoid
+----------------------+

Boom Section Switches:
+----------------------+
| Input -> Digital Pin  |----> BoomSection module
| Mirrors physical state
+----------------------+

Placeholders:
	•	Detailed wiring per boom section
	•	Power ratings per pump/solenoid

⸻

INSTALL.md
	•	Firmware upload via Arduino IDE
	•	Board: Arduino Nano
	•	Port: COMx / /dev/ttyUSBx
	•	Required libraries: Servo.h, Wire.h
	•	Placeholders: library versions, optional GUI integration

⸻

USAGE.md
	1.	Power on system
	2.	Set run/hold switch:
	•	Run: normal operation
	•	Hold: stop pumps, optional bypass solenoid
	3.	Monitor flow sensors per section
	4.	Boom LEDs indicate active sections

	•	Placeholders: flow calibration, safety interlocks, multi-flow sensor expansion notes

⸻

TESTING.md
	•	Bench tests: pump PWM, flow sensor, boom section switching
	•	Synthetic events: simulate run/hold, sensor failures
	•	Calibration: placeholder values for flow mapping and PWM vs flow
	•	Placeholders: expected ranges, bench scripts

⸻

CONFIGURATION.md

Parameter	Default	Description
PWM_MAX	255	Max pump duty
FLOW_TARGET	20 L/min	Target flow per section
SECTION_TIMEOUT	500 ms	Section debounce

	•	Placeholders: additional constants, multi-flow sensor flags

⸻

CHANGELOG.md

Version	Date	Changes
0.1	TBD	Initial prototype firmware
0.2	TBD	Added frozen module interfaces, flow sensor support
0.3	TBD	Run/hold switch and bypass solenoid logic


⸻

LICENSE.md

[Insert license text here]

⸻

AGENTS.md
	•	Automation agents: doc updates, interface validation, test script generation
	•	Placeholders: workflow and execution scripts

⸻

Gaps / Placeholders Remaining
	1.	Detailed wiring per boom section (schematic / graphical optional)
	2.	Module update intervals and loop timing (SEE / THINK / DO)
	3.	Byte-level protocol definitions and error messages
	4.	Flow calibration and PWM mapping values
	5.	Optional BOM for hardware components
	6.	UML diagrams (optional)
	7.	License text