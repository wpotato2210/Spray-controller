#ifndef SPRAY_CONTROLLER_ARDUINO_ADAPTERS_H
#define SPRAY_CONTROLLER_ARDUINO_ADAPTERS_H

#include <array>
#include <stdint.h>

#include "config.h"
#include "interfaces.h"
#include "pins.h"

namespace spray {

class ArduinoActiveLowInput : public DigitalInputAdapter {
 public:
  ArduinoActiveLowInput();
  explicit ArduinoActiveLowInput(uint8_t pin);
  void beginPullup() override;
  bool isActive() const override;

 private:
  uint8_t pin_;
};

class ArduinoActiveHighOutput : public DigitalOutputAdapter {
 public:
  ArduinoActiveHighOutput();
  explicit ArduinoActiveHighOutput(uint8_t pin);
  void beginLow() override;
  void write(bool enabled) override;

 private:
  uint8_t pin_;
};

class ArduinoAnalogInput : public AnalogInputAdapter {
 public:
  explicit ArduinoAnalogInput(uint8_t pin);
  void begin() override;
  int readRaw() const override;

 private:
  uint8_t pin_;
};

class ArduinoPwmOutput : public PwmOutputAdapter {
 public:
  explicit ArduinoPwmOutput(uint8_t pin);
  void begin() override;
  void write(uint8_t duty) override;

 private:
  uint8_t pin_;
};

class ArduinoInterruptPulseCounter : public PulseCounterAdapter {
 public:
  using InterruptHandler = void (*)();

  ArduinoInterruptPulseCounter(uint8_t pin, InterruptHandler handler);
  void begin() override;
  uint32_t readCount() const override;
  uint32_t readLastPulseMs() const override;
  void reset() override;
  void recordPulse();

 private:
  uint8_t pin_;
  InterruptHandler handler_;
  volatile uint32_t pulse_count_;
  volatile uint32_t last_pulse_ms_;
};

class ArduinoSectionHardwareAdapter : public SectionHardwareAdapter {
 public:
  ArduinoSectionHardwareAdapter(
      const std::array<SectionDescriptor, SECTION_COUNT>& sections,
      DigitalOutputAdapter& indicator_output);
  void begin() override;
  bool readSwitch(uint8_t section_id) const override;
  void writeSection(uint8_t section_id, bool enabled) override;

 private:
  const std::array<SectionDescriptor, SECTION_COUNT>& sections_;
  DigitalOutputAdapter& indicator_output_;
  std::array<ArduinoActiveHighOutput, SECTION_COUNT> section_outputs_;
  std::array<ArduinoActiveLowInput, SECTION_COUNT> section_inputs_;
};

ArduinoInterruptPulseCounter& flowPulseCounter();
ArduinoInterruptPulseCounter& wheelPulseCounter();

}  // namespace spray

#endif  // SPRAY_CONTROLLER_ARDUINO_ADAPTERS_H
