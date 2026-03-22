README.md

Spray Controller

Overview

Automated boom sprayer controller for precision agricultural application using a global flow control model.

System Model (authoritative summary)
	•	Single FlowSensor measures total system flow (L/min).
	•	Multiple BoomSection valves are binary ON/OFF outputs.
	•	One global FlowController computes one pump PWM command from target rate, speed, and active width.

Core Components
	•	MCU: Arduino Nano (5V logic)
	•	Pump: 12V PWM-controlled
	•	Boom sections: independent binary valve outputs
	•	Sensors: single YF-S201C flow sensor + wheel speed sensor
	•	Operator control: run/hold switch

OpenSpec Source of Truth
	•	ARCHITECTURE.md → module interfaces
	•	HARDWARE.md → pin mappings
	•	PROTOCOLS.md → message formats
	•	CONFIGURATION.md → constants and units
	•	DEVELOPMENT_ROADMAP.md → phased implementation plan

Quickstart
	1.	Wire hardware according to HARDWARE.md.
	2.	Configure constants from CONFIGURATION.md.
	3.	Install and flash firmware (see INSTALL.md).
	4.	Operate with run/hold and section switches (see USAGE.md).

Dependencies
	•	Arduino IDE
	•	Required libraries: Servo, Wire

Placeholders
	•	High-level project diagram
	•	Supported MCU alternatives beyond Nano (optional)
