#ifndef SPRAY_CONTROLLER_INTERFACES_H
#define SPRAY_CONTROLLER_INTERFACES_H

#include <array>
#include <stdint.h>

#include "config.h"

namespace spray {

class DigitalInputAdapter {
 public:
  virtual ~DigitalInputAdapter() = default;
  virtual void beginPullup() = 0;
  virtual bool isActive() const = 0;
};

class DigitalOutputAdapter {
 public:
  virtual ~DigitalOutputAdapter() = default;
  virtual void beginLow() = 0;
  virtual void write(bool enabled) = 0;
};

class AnalogInputAdapter {
 public:
  virtual ~AnalogInputAdapter() = default;
  virtual void begin() = 0;
  virtual int readRaw() const = 0;
};

class PwmOutputAdapter {
 public:
  virtual ~PwmOutputAdapter() = default;
  virtual void begin() = 0;
  virtual void write(uint8_t duty) = 0;
};

class PulseCounterAdapter {
 public:
  virtual ~PulseCounterAdapter() = default;
  virtual void begin() = 0;
  virtual uint32_t readCount() const = 0;
  virtual uint32_t readLastPulseMs() const = 0;
  virtual void reset() = 0;
};

class TimeSourceAdapter {
 public:
  virtual ~TimeSourceAdapter() = default;
  virtual uint32_t nowMs() const = 0;
};

class SectionHardwareAdapter {
 public:
  virtual ~SectionHardwareAdapter() = default;
  virtual void begin() = 0;
  virtual bool readSwitch(uint8_t section_id) const = 0;
  virtual void writeSection(uint8_t section_id, bool enabled) = 0;
};

class FlowSensor {
 public:
  FlowSensor(PulseCounterAdapter& pulse_counter, TimeSourceAdapter& time_source);
  void begin();
  float readFlow();
  void reset();
  bool isStaleFaultActive() const;
  bool isConfigFaultActive() const;

 private:
  PulseCounterAdapter& pulse_counter_;
  TimeSourceAdapter& time_source_;
  uint32_t last_total_pulses_;
  uint32_t last_read_ms_;
  uint32_t last_pulse_ms_;
  bool stale_fault_active_;
  bool config_fault_active_;
};

class WheelSensor {
 public:
  WheelSensor(PulseCounterAdapter& pulse_counter, TimeSourceAdapter& time_source);
  void begin();
  float readSpeed();
  void reset();
  bool isTimeoutFaultActive() const;
  bool isConfigFaultActive() const;

 private:
  PulseCounterAdapter& pulse_counter_;
  TimeSourceAdapter& time_source_;
  uint32_t last_total_pulses_;
  uint32_t last_read_ms_;
  uint32_t last_pulse_ms_;
  bool timeout_fault_active_;
  bool config_fault_active_;
};

class PressureSensor {
 public:
  explicit PressureSensor(AnalogInputAdapter& analog_input);
  void begin();
  float readPressure();
  void reset();
  bool isConfigFaultActive() const;

 private:
  AnalogInputAdapter& analog_input_;
  bool config_fault_active_;
};

class RunHoldSwitch {
 public:
  explicit RunHoldSwitch(DigitalInputAdapter& input);
  void begin();
  bool readRunHold() const;

 private:
  DigitalInputAdapter& input_;
};

class SectionManager {
 public:
  SectionManager();
  void setSection(uint8_t index, bool state);
  bool getSection(uint8_t index) const;
  uint8_t getActiveCount() const;
  float getActiveWidth() const;

 private:
  std::array<bool, MAX_SECTIONS> sections_;
};

class FlowController {
 public:
  FlowController();
  uint8_t computePumpDuty(float speed_kmh, float active_width_m, float measured_flow_lpm);
  void stop();

 private:
  float current_duty_;
  float flow_integral_;
  float filtered_flow_lpm_;
  float filtered_speed_kmh_;
  uint32_t no_flow_ms_;
  bool flow_filter_initialized_;
  bool speed_filter_initialized_;
};

class PumpControl {
 public:
  explicit PumpControl(PwmOutputAdapter& pwm_output);
  void begin();
  void startPWM();
  void setDutyCycle(uint8_t duty);

 private:
  PwmOutputAdapter& pwm_output_;
};

class CoverageAccumulator {
 public:
  CoverageAccumulator();
  void update(float speed_kmh, float active_width_m, uint32_t elapsed_ms);
  void reset();
  float getDistanceMeters() const;
  float getAreaHectares() const;

 private:
  float distance_m_;
  float area_ha_;
};

}  // namespace spray

#endif  // SPRAY_CONTROLLER_INTERFACES_H
