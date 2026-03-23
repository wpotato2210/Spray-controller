#ifndef SPRAY_CONTROLLER_CALIBRATION_STORE_H
#define SPRAY_CONTROLLER_CALIBRATION_STORE_H

#include <stdint.h>

#include "config.h"

namespace spray {

struct CalibrationProfile {
  float flow_pulses_per_liter;
  float wheel_circumference_m;
  float wheel_pulses_per_rev;
};

class CalibrationStore {
 public:
  CalibrationStore();
  void begin();
  bool load();
  bool save();
  bool setActiveProfile(const CalibrationProfile& profile);
  void resetToDefaults();
  const CalibrationProfile& active() const;

 private:
  struct PersistedCalibrationBlock {
    uint32_t magic;
    CalibrationProfile profile;
    uint16_t checksum;
  };

  static uint16_t computeChecksum(const PersistedCalibrationBlock& block);
  static bool isProfileValid(const CalibrationProfile& profile);

  CalibrationProfile active_profile_;
};

CalibrationStore& calibrationStore();
float activeFlowPulsesPerLiter();
float activeWheelCircumferenceM();
float activeWheelPulsesPerRev();

}  // namespace spray

#endif  // SPRAY_CONTROLLER_CALIBRATION_STORE_H
