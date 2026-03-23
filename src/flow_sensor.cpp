#include "interfaces.h"

#include <Arduino.h>

#include "config.h"
#include "calibration_store.h"

namespace spray {

FlowSensor::FlowSensor(PulseCounterAdapter& pulse_counter)
    : pulse_counter_(pulse_counter),
      last_total_pulses_(0U),
      last_read_ms_(0U),
      last_pulse_ms_(0U),
      stale_fault_active_(false),
      config_fault_active_(false) {}

void FlowSensor::begin() {
  pulse_counter_.begin();
  last_read_ms_ = millis();
  last_pulse_ms_ = last_read_ms_;
  pulse_counter_.reset();
}

float FlowSensor::readFlow() {
  const uint32_t now_ms = millis();
  const uint32_t elapsed_ms = now_ms - last_read_ms_;
  if (elapsed_ms == 0U) {
    return 0.0f;
  }

  const uint32_t total_pulses = pulse_counter_.readCount();
  const uint32_t last_pulse_ms = pulse_counter_.readLastPulseMs();
  const uint32_t delta_pulses = total_pulses - last_total_pulses_;
  const bool pulse_timed_out = (now_ms - last_pulse_ms) >= FLOW_STALE_TIMEOUT_MS;
  const float elapsed_s = static_cast<float>(elapsed_ms) / 1000.0f;
  stale_fault_active_ = pulse_timed_out;
  const float flow_pulses_per_liter = activeFlowPulsesPerLiter();
  config_fault_active_ = (flow_pulses_per_liter <= 0.0f || elapsed_s <= 0.0f);
  if (config_fault_active_) {
    last_total_pulses_ = total_pulses;
    last_read_ms_ = now_ms;
    last_pulse_ms_ = last_pulse_ms;
    return FLOW_STALE_FALLBACK_LPM;
  }

  if (pulse_timed_out) {
    last_total_pulses_ = total_pulses;
    last_read_ms_ = now_ms;
    last_pulse_ms_ = last_pulse_ms;
    return FLOW_STALE_FALLBACK_LPM;
  }

  if (delta_pulses == 0U) {
    last_read_ms_ = now_ms;
    last_pulse_ms_ = last_pulse_ms;
    return 0.0f;
  }
  const float pulse_freq_hz = static_cast<float>(delta_pulses) / elapsed_s;
  float flow_lpm = (pulse_freq_hz / flow_pulses_per_liter) * 60.0f;

  if (flow_lpm < 0.0f) {
    flow_lpm = 0.0f;
  }
  if (flow_lpm > MAX_FLOW_LPM) {
    flow_lpm = MAX_FLOW_LPM;
  }

  last_total_pulses_ = total_pulses;
  last_read_ms_ = now_ms;
  last_pulse_ms_ = last_pulse_ms;
  return flow_lpm;
}

void FlowSensor::reset() {
  pulse_counter_.reset();
  last_total_pulses_ = 0U;
  last_read_ms_ = millis();
  last_pulse_ms_ = last_read_ms_;
  stale_fault_active_ = false;
  config_fault_active_ = false;
}

bool FlowSensor::isStaleFaultActive() const { return stale_fault_active_; }

bool FlowSensor::isConfigFaultActive() const { return config_fault_active_; }

}  // namespace spray
