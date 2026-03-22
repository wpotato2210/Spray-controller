#!/usr/bin/env bash
set -euo pipefail

: "${FQBN:?FQBN is required, e.g. arduino:avr:nano}"

EXTRA_INCLUDE="-I${GITHUB_WORKSPACE:-$(pwd)}/include"

arduino-cli compile \
  --fqbn "$FQBN" \
  --build-property "compiler.cpp.extra_flags=${EXTRA_INCLUDE}" \
  --build-property "compiler.c.extra_flags=${EXTRA_INCLUDE}" \
  ./Spray-controller.ino
