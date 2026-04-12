# WAVE-04 HIL Validation Protocol

Date baseline: 2026-04-12

## Scope

Repeatable hardware-in-loop protocol for:

1. ISR pulse-rate behavior
2. Serial saturation behavior
3. Switch bounce behavior

## Preconditions

- Target board flashed with current `main` firmware.
- Serial monitor connected at documented baud rate.
- Bench power rails and section outputs instrumented.
- Optional: logic analyzer for pulse/switch traces.

## Test Matrix

| Case ID | Scenario | Procedure | Pass Criteria |
| --- | --- | --- | --- |
| HIL-ISR-001 | Wheel pulse low/mid/high rates | Inject wheel pulses at representative rates (e.g., 5 Hz, 50 Hz, 200 Hz) for 30 s each. | `ST:` speed remains finite, monotonic with pulse rate, and drops to 0 after timeout when pulses stop. |
| HIL-ISR-002 | Flow pulse stale detection | Inject flow pulses, then stop injection for > `FLOW_STALE_TIMEOUT_MS`. | Flow output falls to `FLOW_STALE_FALLBACK_LPM` and stale-fault telemetry path is asserted. |
| HIL-SER-001 | Serial ingress saturation | Stream command traffic exceeding `SERIAL_INGRESS_BUDGET_BYTES_PER_LOOP` while telemetry is enabled. | Loop remains responsive; no lockup/reset; bounded ingress behavior observed over 60 s. |
| HIL-SER-002 | Telemetry budget under load | Force high telemetry pressure (all sections active + sensor updates). | Emission respects frame budgeting (`TELEMETRY_FRAME_BUDGET_PER_LOOP`), no unbounded burst behavior. |
| HIL-SW-001 | Run/Hold bounce | Inject synthetic bounce on run/hold input (rapid toggles within debounce window). | Output state changes only after debounce interval; no pump chatter. |
| HIL-SW-002 | Section switch bounce | Inject bounce on each section switch independently. | Section outputs remain stable and transition only on debounced state. |

## Execution Record Template

For each case:

- Timestamp (UTC)
- Board target (`arduino:avr:uno|nano|mega`)
- Input profile (pulse/switch/serial generator settings)
- Observed telemetry snippets (`ST:`, `RS:`)
- Pass/Fail
- Notes or anomalies

## Exit Criteria

- All six cases pass on at least one supported board.
- At least one ISR and one switch-bounce case are repeated on a second board target.
- Any failure has a linked issue and explicit mitigation decision before release gating.
