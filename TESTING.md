# TESTING

Spray Controller Testing Procedures

## Bench Tests

- Verify pump PWM signal
- Verify single flow sensor pulse reading for total flow
- Verify boom section switch and LED response
- Verify run/hold switch behavior

## Synthetic Event Tests

- Simulate run/hold switch toggle
- Simulate total-flow sensor dropouts and spikes
- Toggle section combinations and verify active width

## Calibration Procedures

- Map PWM duty cycle to total flow rate
- Verify flow readings against known volume over time
- Validate rate-control response using speed, width, and target rate

## Placeholders

- Expected total-flow ranges
- Calibration tables and formulas
- Bench automation scripts
