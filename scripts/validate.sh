#!/usr/bin/env bash
set -euo pipefail

YAMLLINT_BIN="yamllint"
if ! command -v yamllint >/dev/null 2>&1; then
  YAMLLINT_BIN="scripts/yamllint"
fi

run_step() {
  local name="$1"
  shift
  echo "== $name =="
  if ! "$@"; then
    echo "FAIL: $name"
    exit 1
  fi
}

run_step "YAML schema validation" \
  "${YAMLLINT_BIN}" -c .yamllint.yml \
  config.yaml openspec.yaml pins.yaml protocols.yaml validation.yaml

run_step "Protocol structure check" \
  python3 scripts/validate_protocols.py

run_step "Config completeness check" \
  python3 scripts/validate_config.py

run_step "PIN-WAVE-001 pin-map check" \
  python3 scripts/validate_pin_wave_001.py

run_step "PIN-WAVE-002 parity check" \
  python3 scripts/validate_pin_wave_002.py

run_step "PIN-WAVE-003 Mega policy check" \
  python3 scripts/validate_pin_wave_003.py

run_step "PIN-WAVE-004 hardware doc parity check" \
  python3 scripts/validate_pin_wave_004.py

run_step "W-01 memory/pin/doc closure check" \
  python3 scripts/validate_wave_w_01.py

run_step "W-02 timing/event closure check" \
  python3 scripts/validate_wave_w_02.py

run_step "W-03 HAL/fault closure check" \
  python3 scripts/validate_wave_w_03.py

run_step "WAVE-02 firmware stability check" \
  python3 scripts/validate_fw_wave_02.py

run_step "P3 sensor robustness check" \
  python3 scripts/validate_p3_sensor_robustness.py

run_step "P3 closure artifact check" \
  python3 scripts/validate_p3_closure.py

run_step "P0 closure artifact check" \
  python3 scripts/validate_p0_closure.py

run_step "P1 closure artifact check" \
  python3 scripts/validate_p1_closure.py

run_step "P2 closure artifact check" \
  python3 scripts/validate_p2_closure.py

run_step "P4 closure artifact check" \
  python3 scripts/validate_p4_closure.py

run_step "P4 operator interface check" \
  python3 scripts/validate_p4_operator_interface.py

run_step "P5 scalability invariant check" \
  python3 scripts/validate_p5_scalability.py

run_step "P5 closure artifact check" \
  python3 scripts/validate_p5_closure.py

run_step "P6 flow calibration workflow check" \
  python3 scripts/validate_p6_flow_calibration.py

run_step "P6 wheel calibration workflow check" \
  python3 scripts/validate_p6_wheel_calibration.py

run_step "P6 calibration storage check" \
  python3 scripts/validate_p6_calibration_storage.py

run_step "P6 calibration override check" \
  python3 scripts/validate_p6_calibration_override.py

run_step "P6 closure artifact check" \
  python3 scripts/validate_p6_closure.py

run_step "P7 documentation alignment check" \
  python3 scripts/validate_p7_documentation.py

run_step "WAVE-04 static analysis check" \
  python3 scripts/validate_wave_04_static.py

run_step "WAVE-04 native unit check" \
  bash scripts/run_wave_04_unit_tests.sh

echo "== SUCCESS =="
