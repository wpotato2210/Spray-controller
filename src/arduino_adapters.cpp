#include "arduino_adapters.h"

#include <Arduino.h>

namespace spray {
namespace {
void onFlowPulse();
void onWheelPulse();

ArduinoInterruptPulseCounter g_flow_pulse_counter(PIN_FLOW_SENSOR, onFlowPulse);
ArduinoInterruptPulseCounter g_wheel_pulse_counter(PIN_WHEEL_SENSOR, onWheelPulse);

void onFlowPulse() { g_flow_pulse_counter.recordPulse(); }
void onWheelPulse() { g_wheel_pulse_counter.recordPulse(); }
}  // namespace

ArduinoActiveLowInput::ArduinoActiveLowInput(uint8_t pin) : pin_(pin) {}

void ArduinoActiveLowInput::beginPullup() { pinMode(pin_, INPUT_PULLUP); }

bool ArduinoActiveLowInput::isActive() const { return digitalRead(pin_) == LOW; }

ArduinoActiveHighOutput::ArduinoActiveHighOutput(uint8_t pin) : pin_(pin) {}

void ArduinoActiveHighOutput::beginLow() {
  pinMode(pin_, OUTPUT);
  digitalWrite(pin_, LOW);
}

void ArduinoActiveHighOutput::write(bool enabled) {
  digitalWrite(pin_, enabled ? HIGH : LOW);
}

ArduinoAnalogInput::ArduinoAnalogInput(uint8_t pin) : pin_(pin) {}

void ArduinoAnalogInput::begin() { pinMode(pin_, INPUT); }

int ArduinoAnalogInput::readRaw() const { return analogRead(pin_); }

ArduinoPwmOutput::ArduinoPwmOutput(uint8_t pin) : pin_(pin) {}

void ArduinoPwmOutput::begin() { pinMode(pin_, OUTPUT); }

void ArduinoPwmOutput::write(uint8_t duty) { analogWrite(pin_, duty); }

ArduinoInterruptPulseCounter::ArduinoInterruptPulseCounter(uint8_t pin, InterruptHandler handler)
    : pin_(pin), handler_(handler), pulse_count_(0U), last_pulse_ms_(0U) {}

void ArduinoInterruptPulseCounter::begin() {
  pinMode(pin_, INPUT_PULLUP);
  const int interrupt_id = digitalPinToInterrupt(pin_);
  if (interrupt_id != NOT_AN_INTERRUPT && handler_ != nullptr) {
    attachInterrupt(interrupt_id, handler_, RISING);
  }
  last_pulse_ms_ = millis();
}

uint32_t ArduinoInterruptPulseCounter::readCount() const {
  noInterrupts();
  const uint32_t count = pulse_count_;
  interrupts();
  return count;
}

uint32_t ArduinoInterruptPulseCounter::readLastPulseMs() const {
  noInterrupts();
  const uint32_t last_pulse_ms = last_pulse_ms_;
  interrupts();
  return last_pulse_ms;
}

void ArduinoInterruptPulseCounter::reset() {
  noInterrupts();
  pulse_count_ = 0U;
  last_pulse_ms_ = millis();
  interrupts();
}

void ArduinoInterruptPulseCounter::recordPulse() {
  ++pulse_count_;
  last_pulse_ms_ = millis();
}

ArduinoSectionHardwareAdapter::ArduinoSectionHardwareAdapter(
    const std::array<SectionDescriptor, SECTION_COUNT>& sections,
    DigitalOutputAdapter& indicator_output)
    : sections_(sections), indicator_output_(indicator_output) {}

void ArduinoSectionHardwareAdapter::begin() {
  for (const SectionDescriptor& section : sections_) {
    ArduinoActiveHighOutput output(section.output_pin);
    ArduinoActiveLowInput input(section.switch_pin);
    output.beginLow();
    input.beginPullup();
  }
  indicator_output_.beginLow();
}

bool ArduinoSectionHardwareAdapter::readSwitch(uint8_t section_id) const {
  for (const SectionDescriptor& section : sections_) {
    if (section.id == section_id) {
      return ArduinoActiveLowInput(section.switch_pin).isActive();
    }
  }
  return false;
}

void ArduinoSectionHardwareAdapter::writeSection(uint8_t section_id, bool enabled) {
  for (const SectionDescriptor& section : sections_) {
    if (section.id == section_id) {
      ArduinoActiveHighOutput(section.output_pin).write(enabled);
      break;
    }
  }
  if (section_id == 0U) {
    indicator_output_.write(enabled);
  }
}

ArduinoInterruptPulseCounter& flowPulseCounter() { return g_flow_pulse_counter; }
ArduinoInterruptPulseCounter& wheelPulseCounter() { return g_wheel_pulse_counter; }

}  // namespace spray
