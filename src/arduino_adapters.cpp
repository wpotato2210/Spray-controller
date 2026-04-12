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

ArduinoActiveLowInput::ArduinoActiveLowInput() : pin_(0U) {}

ArduinoActiveLowInput::ArduinoActiveLowInput(uint8_t pin) : pin_(pin) {}

void ArduinoActiveLowInput::beginPullup() { pinMode(pin_, INPUT_PULLUP); }

bool ArduinoActiveLowInput::isActive() const { return digitalRead(pin_) == LOW; }

ArduinoActiveHighOutput::ArduinoActiveHighOutput() : pin_(0U) {}

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

PulseCounterSnapshot ArduinoInterruptPulseCounter::readSnapshot() const {
  noInterrupts();
  const PulseCounterSnapshot snapshot{pulse_count_, last_pulse_ms_};
  interrupts();
  return snapshot;
}

uint32_t ArduinoInterruptPulseCounter::readCount() const {
  return readSnapshot().count;
}

uint32_t ArduinoInterruptPulseCounter::readLastPulseMs() const {
  return readSnapshot().last_pulse_ms;
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

uint32_t ArduinoMillisTimeSource::nowMs() const { return millis(); }

ArduinoSectionHardwareAdapter::ArduinoSectionHardwareAdapter(
    const std::array<SectionDescriptor, SECTION_COUNT>& sections,
    DigitalOutputAdapter& indicator_output)
    : sections_(sections),
      indicator_output_(indicator_output),
      section_outputs_{},
      section_inputs_{} {
  for (size_t index = 0; index < SECTION_COUNT; ++index) {
    section_outputs_[index] = ArduinoActiveHighOutput(sections_[index].output_pin);
    section_inputs_[index] = ArduinoActiveLowInput(sections_[index].switch_pin);
  }
}

void ArduinoSectionHardwareAdapter::begin() {
  for (size_t index = 0; index < SECTION_COUNT; ++index) {
    section_outputs_[index].beginLow();
    section_inputs_[index].beginPullup();
  }
  indicator_output_.beginLow();
}

bool ArduinoSectionHardwareAdapter::readSwitch(uint8_t section_id) const {
  for (size_t index = 0; index < SECTION_COUNT; ++index) {
    if (sections_[index].id == section_id) {
      return section_inputs_[index].isActive();
    }
  }
  return false;
}

void ArduinoSectionHardwareAdapter::writeSection(uint8_t section_id, bool enabled) {
  for (size_t index = 0; index < SECTION_COUNT; ++index) {
    if (sections_[index].id == section_id) {
      section_outputs_[index].write(enabled);
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
