#!/usr/bin/env bash
set -euo pipefail

ARDUINO_CORE="${ARDUINO_CORE:-arduino:avr}"

if ! command -v arduino-cli >/dev/null 2>&1; then
  tmp_dir="$(mktemp -d)"
  trap 'rm -rf "$tmp_dir"' EXIT
  curl -fsSL https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz -o "$tmp_dir/arduino-cli.tar.gz"
  tar -xzf "$tmp_dir/arduino-cli.tar.gz" -C "$tmp_dir"
  install "$tmp_dir/arduino-cli" /usr/local/bin/arduino-cli
fi

arduino-cli config init --overwrite
arduino-cli core update-index
arduino-cli core install "$ARDUINO_CORE"
