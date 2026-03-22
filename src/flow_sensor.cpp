#include "interfaces.h"

#include <Arduino.h>

#include "config.h"

namespace spray {
namespace {
volatile uint32_t g_flow_pulses = 0U;
volatile uint32_t g_last_flow_pulse_ms = 0U;

void onFlowPulse() {
  ++g_flow_pulses;
  g_last_flow_pulse_ms = millis();
}
}  // namespace

FlowSensor::FlowSensor(uint8_t pin)
    : pin_(pin),
      last_total_pulses_(0U),
      last_read_ms_(0U),
      last_pulse_ms_(0U),
      stale_fault_active_(false),
      config_fault_active_(false) {}

void FlowSensor::begin() {
  pinMode(pin_, INPUT_PULLUP);
  const int interrupt_id = digitalPinToInterrupt(pin_);
  if (interrupt_id != NOT_AN_INTERRUPT) {
    attachInterrupt(interrupt_id, onFlowPulse, RISING);
  }
  last_read_ms_ = millis();
  last_pulse_ms_ = last_read_ms_;
  noInterrupts();
  g_last_flow_pulse_ms = last_pulse_ms_;
  interrupts();
}

float FlowSensor::readFlow() {
  const uint32_t now_ms = millis();
  const uint32_t elapsed_ms = now_ms - last_read_ms_;
  if (elapsed_ms == 0U) {
    return 0.0f;
  }

  noInterrupts();
  const uint32_t total_pulses = g_flow_pulses;
  const uint32_t last_pulse_ms = g_last_flow_pulse_ms;
  interrupts();

  const uint32_t delta_pulses = total_pulses - last_total_pulses_;
  const bool pulse_timed_out = (now_ms - last_pulse_ms) >= FLOW_STALE_TIMEOUT_MS;
  const float elapsed_s = static_cast<float>(elapsed_ms) / 1000.0f;
  stale_fault_active_ = pulse_timed_out;
  config_fault_active_ = (FLOW_PULSES_PER_LITER <= 0.0f || elapsed_s <= 0.0f);
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
  float flow_lpm = (pulse_freq_hz / FLOW_PULSES_PER_LITER) * 60.0f;

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
  noInterrupts();
  g_flow_pulses = 0U;
  g_last_flow_pulse_ms = millis();
  interrupts();

  last_total_pulses_ = 0U;
  last_read_ms_ = millis();
  last_pulse_ms_ = last_read_ms_;
  stale_fault_active_ = false;
  config_fault_active_ = false;
}

bool FlowSensor::isStaleFaultActive() const { return stale_fault_active_; }

bool FlowSensor::isConfigFaultActive() const { return config_fault_active_; }

}  // namespace spray
