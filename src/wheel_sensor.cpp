#include "interfaces.h"

#include <Arduino.h>

#include "config.h"

namespace spray {
namespace {
volatile uint32_t g_wheel_pulses = 0U;
volatile uint32_t g_last_wheel_pulse_ms = 0U;

void onWheelPulse() {
  ++g_wheel_pulses;
  g_last_wheel_pulse_ms = millis();
}
}  // namespace

WheelSensor::WheelSensor(uint8_t pin)
    : pin_(pin),
      last_total_pulses_(0U),
      last_read_ms_(0U),
      last_pulse_ms_(0U),
      timeout_fault_active_(false),
      config_fault_active_(false) {}

void WheelSensor::begin() {
  pinMode(pin_, INPUT_PULLUP);
  const int interrupt_id = digitalPinToInterrupt(pin_);
  if (interrupt_id != NOT_AN_INTERRUPT) {
    attachInterrupt(interrupt_id, onWheelPulse, RISING);
  }
  last_read_ms_ = millis();
  last_pulse_ms_ = last_read_ms_;
  noInterrupts();
  g_last_wheel_pulse_ms = last_pulse_ms_;
  interrupts();
}

float WheelSensor::readSpeed() {
  const uint32_t now_ms = millis();
  const uint32_t elapsed_ms = now_ms - last_read_ms_;
  if (elapsed_ms == 0U) {
    return 0.0f;
  }

  noInterrupts();
  const uint32_t total_pulses = g_wheel_pulses;
  const uint32_t last_pulse_ms = g_last_wheel_pulse_ms;
  interrupts();

  const uint32_t delta_pulses = total_pulses - last_total_pulses_;
  const bool pulse_timed_out = (now_ms - last_pulse_ms) >= WHEEL_PULSE_TIMEOUT_MS;
  const float elapsed_s = static_cast<float>(elapsed_ms) / 1000.0f;
  timeout_fault_active_ = pulse_timed_out;
  config_fault_active_ = (WHEEL_PULSES_PER_REV <= 0.0f || elapsed_s <= 0.0f);
  if (config_fault_active_ || pulse_timed_out) {
    last_total_pulses_ = total_pulses;
    last_read_ms_ = now_ms;
    last_pulse_ms_ = last_pulse_ms;
    return 0.0f;
  }
  const float pulse_freq_hz = static_cast<float>(delta_pulses) / elapsed_s;

  float speed_kmh =
      (pulse_freq_hz / WHEEL_PULSES_PER_REV) * WHEEL_CIRCUMFERENCE_M * 3.6f;

  if (speed_kmh < 0.0f) {
    speed_kmh = 0.0f;
  }

  last_total_pulses_ = total_pulses;
  last_read_ms_ = now_ms;
  last_pulse_ms_ = last_pulse_ms;
  return speed_kmh;
}

void WheelSensor::reset() {
  noInterrupts();
  g_wheel_pulses = 0U;
  g_last_wheel_pulse_ms = millis();
  interrupts();

  last_total_pulses_ = 0U;
  last_read_ms_ = millis();
  last_pulse_ms_ = last_read_ms_;
  timeout_fault_active_ = false;
  config_fault_active_ = false;
}

bool WheelSensor::isTimeoutFaultActive() const { return timeout_fault_active_; }

bool WheelSensor::isConfigFaultActive() const { return config_fault_active_; }

}  // namespace spray
