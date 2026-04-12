#!/usr/bin/env bash
set -euo pipefail

mkdir -p build

g++ -std=c++17 -I. -Iinclude \
  tests/native/test_wave_04_quality.cpp \
  src/flow_sensor.cpp \
  src/wheel_sensor.cpp \
  src/flow_controller.cpp \
  -o build/test_wave_04_quality

./build/test_wave_04_quality
