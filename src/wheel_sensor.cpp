#include "interfaces.h"

#include <Arduino.h>

#include "config.h"
#include "calibration_store.h"

namespace spray {

WheelSensor::WheelSensor(PulseCounterAdapter& pulse_counter)
    : pulse_counter_(pulse_counter),
      last_total_pulses_(0U),
      last_read_ms_(0U),
      last_pulse_ms_(0U),
      timeout_fault_active_(false),
      config_fault_active_(false) {}

void WheelSensor::begin() {
  pulse_counter_.begin();
  last_read_ms_ = millis();
  last_pulse_ms_ = last_read_ms_;
  pulse_counter_.reset();
}

float WheelSensor::readSpeed() {
  const uint32_t now_ms = millis();
  const uint32_t elapsed_ms = now_ms - last_read_ms_;
  if (elapsed_ms == 0U) {
    return 0.0f;
  }

  const uint32_t total_pulses = pulse_counter_.readCount();
  const uint32_t last_pulse_ms = pulse_counter_.readLastPulseMs();

  const uint32_t delta_pulses = total_pulses - last_total_pulses_;
  const bool pulse_timed_out = (now_ms - last_pulse_ms) >= WHEEL_PULSE_TIMEOUT_MS;
  const float elapsed_s = static_cast<float>(elapsed_ms) / 1000.0f;
  timeout_fault_active_ = pulse_timed_out;
  const float wheel_pulses_per_rev = activeWheelPulsesPerRev();
  const float wheel_circumference_m = activeWheelCircumferenceM();
  config_fault_active_ =
      (wheel_pulses_per_rev <= 0.0f || wheel_circumference_m <= 0.0f || elapsed_s <= 0.0f);
  if (config_fault_active_ || pulse_timed_out) {
    last_total_pulses_ = total_pulses;
    last_read_ms_ = now_ms;
    last_pulse_ms_ = last_pulse_ms;
    return 0.0f;
  }
  const float pulse_freq_hz = static_cast<float>(delta_pulses) / elapsed_s;

  float speed_kmh =
      (pulse_freq_hz / wheel_pulses_per_rev) * wheel_circumference_m * 3.6f;

  if (speed_kmh < 0.0f) {
    speed_kmh = 0.0f;
  }

  last_total_pulses_ = total_pulses;
  last_read_ms_ = now_ms;
  last_pulse_ms_ = last_pulse_ms;
  return speed_kmh;
}

void WheelSensor::reset() {
  pulse_counter_.reset();
  last_total_pulses_ = 0U;
  last_read_ms_ = millis();
  last_pulse_ms_ = last_read_ms_;
  timeout_fault_active_ = false;
  config_fault_active_ = false;
}

bool WheelSensor::isTimeoutFaultActive() const { return timeout_fault_active_; }

bool WheelSensor::isConfigFaultActive() const { return config_fault_active_; }

}  // namespace spray
