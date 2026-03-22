#!/usr/bin/env bash
set -euo pipefail

mkdir -p validation

if ! bash scripts/validate.sh | tee validation/validation_pass.txt; then
  echo "VALIDATION_FAILED" | tee -a validation/validation_pass.txt
  exit 1
fi