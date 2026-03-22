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

echo "== SUCCESS =="
