#include "interfaces.h"

#include <Arduino.h>

#include "config.h"

namespace spray {
namespace {
volatile uint32_t g_wheel_pulses = 0U;

void onWheelPulse() { ++g_wheel_pulses; }
}  // namespace

WheelSensor::WheelSensor(uint8_t pin)
    : pin_(pin), last_total_pulses_(0U), last_read_ms_(0U) {}

void WheelSensor::begin() {
  pinMode(pin_, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_), onWheelPulse, RISING);
  last_read_ms_ = millis();
}

float WheelSensor::readSpeed() {
  const uint32_t now_ms = millis();
  const uint32_t elapsed_ms = now_ms - last_read_ms_;
  if (elapsed_ms == 0U) {
    return 0.0f;
  }

  noInterrupts();
  const uint32_t total_pulses = g_wheel_pulses;
  interrupts();

  const uint32_t delta_pulses = total_pulses - last_total_pulses_;
  const float elapsed_s = static_cast<float>(elapsed_ms) / 1000.0f;
  const float pulse_freq_hz = static_cast<float>(delta_pulses) / elapsed_s;

  float speed_kmh =
      (pulse_freq_hz / WHEEL_PULSES_PER_REV) * WHEEL_CIRCUMFERENCE_M * 3.6f;

  if (speed_kmh < 0.0f) {
    speed_kmh = 0.0f;
  }

  last_total_pulses_ = total_pulses;
  last_read_ms_ = now_ms;
  return speed_kmh;
}

void WheelSensor::reset() {
  noInterrupts();
  g_wheel_pulses = 0U;
  interrupts();

  last_total_pulses_ = 0U;
  last_read_ms_ = millis();
}

}  // namespace spray
