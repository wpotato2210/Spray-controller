#!/usr/bin/env bash
set -euo pipefail

YAMLLINT_BIN="yamllint"
if ! command -v yamllint >/dev/null 2>&1; then
  YAMLLINT_BIN="scripts/yamllint"
fi

printf '== YAML schema validation ==\n'
"${YAMLLINT_BIN}" -c .yamllint.yml \
  config.yaml \
  openspec.yaml \
  pins.yaml \
  protocols.yaml \
  validation.yaml

printf '== Protocol structure check ==\n'
python3 scripts/validate_protocols.py

printf '== Config completeness check ==\n'
python3 scripts/validate_config.py

printf '== SUCCESS ==\n'
