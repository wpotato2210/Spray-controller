TESTING.md

Spray Controller Testing Procedures

Bench Tests
	•	Pump PWM signal verification.
	•	Single flow sensor pulse reading verification (total flow only).
	•	Boom section switch and LED response.
	•	Run/Hold switch behavior verification.

Synthetic Event Tests
	•	Simulate run/hold switch toggle.
	•	Simulate total-flow sensor dropouts/spikes.
	•	Toggle section combinations and verify active width calculation.

Calibration Procedures
	•	Map PWM duty cycle to total flow rate.
	•	Verify total flow sensor readings against known volume over time.
	•	Validate rate-control response using speed (km/h), width (m), and target rate (L/ha).

Placeholders
	•	Expected total-flow ranges
	•	Calibration tables and formulas
	•	Bench test scripts for automation
