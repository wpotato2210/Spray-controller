#include "interfaces.h"

#include <Arduino.h>

#include "config.h"

namespace spray {
namespace {
volatile uint32_t g_flow_pulses = 0U;

void onFlowPulse() { ++g_flow_pulses; }
}  // namespace

FlowSensor::FlowSensor(uint8_t pin)
    : pin_(pin), last_total_pulses_(0U), last_read_ms_(0U) {}

void FlowSensor::begin() {
  pinMode(pin_, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_), onFlowPulse, RISING);
  last_read_ms_ = millis();
}

float FlowSensor::readFlow() {
  const uint32_t now_ms = millis();
  const uint32_t elapsed_ms = now_ms - last_read_ms_;
  if (elapsed_ms == 0U) {
    return 0.0f;
  }

  noInterrupts();
  const uint32_t total_pulses = g_flow_pulses;
  interrupts();

  const uint32_t delta_pulses = total_pulses - last_total_pulses_;
  const float elapsed_s = static_cast<float>(elapsed_ms) / 1000.0f;
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
  return flow_lpm;
}

void FlowSensor::reset() {
  noInterrupts();
  g_flow_pulses = 0U;
  interrupts();

  last_total_pulses_ = 0U;
  last_read_ms_ = millis();
}

}  // namespace spray
