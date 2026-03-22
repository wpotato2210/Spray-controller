#!/usr/bin/env bash
set -euo pipefail

mkdir -p validation
bash scripts/validate.sh > validation/validation_pass.txt
