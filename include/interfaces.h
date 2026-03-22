#ifndef SPRAY_CONTROLLER_INTERFACES_H
#define SPRAY_CONTROLLER_INTERFACES_H

#include <stdint.h>

#include "config.h"

namespace spray {

class FlowSensor {
 public:
  explicit FlowSensor(uint8_t pin);
  void begin();
  float readFlow();
  void reset();

 private:
  uint8_t pin_;
  uint32_t last_total_pulses_;
  uint32_t last_read_ms_;
};

class WheelSensor {
 public:
  explicit WheelSensor(uint8_t pin);
  void begin();
  float readSpeed();
  void reset();

 private:
  uint8_t pin_;
  uint32_t last_total_pulses_;
  uint32_t last_read_ms_;
};

class RunHoldSwitch {
 public:
  explicit RunHoldSwitch(uint8_t pin);
  void begin();
  bool readRunHold() const;

 private:
  uint8_t pin_;
};

class SectionManager {
 public:
  SectionManager();
  void setSection(uint8_t index, bool state);
  bool getSection(uint8_t index) const;
  uint8_t getActiveCount() const;
  float getActiveWidth() const;

 private:
  bool sections_[SECTION_COUNT];
};

class FlowController {
 public:
  FlowController();
  uint8_t computePumpDuty(float speed_kmh, float active_width_m, float measured_flow_lpm);
  void stop();

 private:
  float current_duty_;
};

class PumpControl {
 public:
  explicit PumpControl(uint8_t pin);
  void begin();
  void startPWM();
  void stopPWM();
  void setDutyCycle(uint8_t duty);

 private:
  uint8_t pin_;
};

}  // namespace spray

#endif  // SPRAY_CONTROLLER_INTERFACES_H
