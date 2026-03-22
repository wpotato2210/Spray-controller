#!/usr/bin/env bash
set -euo pipefail

: "${FQBN:?FQBN is required, e.g. arduino:avr:nano}"

EXTRA_INCLUDE="-I${GITHUB_WORKSPACE:-$(pwd)}/include"
LOG_FILE="${LOG_FILE:-}"

COMPILE_CMD=(
  arduino-cli compile
  --fqbn "$FQBN"
  --build-property "compiler.cpp.extra_flags=${EXTRA_INCLUDE}"
  --build-property "compiler.c.extra_flags=${EXTRA_INCLUDE}"
  ./Spray-controller.ino
)

if [[ -n "$LOG_FILE" ]]; then
  mkdir -p "$(dirname "$LOG_FILE")"
  "${COMPILE_CMD[@]}" 2>&1 | tee "$LOG_FILE"
else
  "${COMPILE_CMD[@]}"
fi
