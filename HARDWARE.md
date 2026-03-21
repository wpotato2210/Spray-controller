HARDWARE.md

Spray Controller Hardware

Hardware Summary
	•	Single flow sensor for total flow feedback.
	•	Multiple boom sections controlled independently as ON/OFF outputs.
	•	12V pump power stage with 5V MCU logic.

Pinout

Pin	Component	Notes
2	Wheel Sensor	Digital pulse input
3	Flow Sensor	Single total-flow pulse input (YF-S201C)
11	Pump PWM	PWM output to 12V pump driver
12	Boom Section 1	Digital output (ON/OFF)
13	Boom Section 2	Digital output (ON/OFF)
A1	Boom Section 3	Digital output (ON/OFF)
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
| 3  -> Flow Sensor    |----> Single total-flow pulse input
| 11 -> Pump PWM       |----> 12V Pump via N-MOSFET / Relay
| 12 -> Boom 1         |----> Solenoid 1 (ON/OFF)
| 13 -> Boom 2         |----> Solenoid 2 (ON/OFF)
| A1 -> Boom 3         |----> Solenoid 3 (ON/OFF)
| A2 -> LED Boom 1     |----> Indicator LED
| GND -----------------|----> Common GND
| 5V ------------------|----> MCU logic supply
+----------------------+

Placeholders:
	•	Detailed wiring per boom section
	•	Power ratings per pump/solenoid
