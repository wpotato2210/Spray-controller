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

run_step "P3 sensor robustness check" \
  python3 scripts/validate_p3_sensor_robustness.py

run_step "P4 operator interface check" \
  python3 scripts/validate_p4_operator_interface.py

run_step "P5 scalability invariant check" \
  python3 scripts/validate_p5_scalability.py

run_step "P6 flow calibration workflow check" \
  python3 scripts/validate_p6_flow_calibration.py

run_step "P6 wheel calibration workflow check" \
  python3 scripts/validate_p6_wheel_calibration.py

run_step "P6 calibration storage check" \
  python3 scripts/validate_p6_calibration_storage.py

run_step "P6 calibration override check" \
  python3 scripts/validate_p6_calibration_override.py

run_step "P7 documentation alignment check" \
  python3 scripts/validate_p7_documentation.py

echo "== SUCCESS =="
