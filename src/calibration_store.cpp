#include "calibration_store.h"

#include <EEPROM.h>
#include <string.h>

namespace spray {
namespace {
constexpr int kCalibrationStorageAddress = 0;

CalibrationProfile defaultCalibrationProfile() {
  return CalibrationProfile{
      FLOW_PULSES_PER_LITER,
      WHEEL_CIRCUMFERENCE_M,
      WHEEL_PULSES_PER_REV,
  };
}

CalibrationStore g_calibration_store;
}  // namespace

CalibrationStore::CalibrationStore()
    : active_profile_(defaultCalibrationProfile()), use_defaults_override_(false) {}

void CalibrationStore::begin() {}

bool CalibrationStore::load() {
  PersistedCalibrationBlock block{};
  EEPROM.get(kCalibrationStorageAddress, block);
  const bool has_valid_magic = block.magic == CALIBRATION_STORAGE_MAGIC;
  const bool has_valid_checksum = block.checksum == computeChecksum(block);
  const bool has_valid_profile = isProfileValid(block.profile);
  if (!has_valid_magic || !has_valid_checksum || !has_valid_profile) {
    resetToDefaults();
    return false;
  }

  active_profile_ = block.profile;
  use_defaults_override_ = block.use_defaults_override != 0U;
  return true;
}

bool CalibrationStore::save() {
  if (!isProfileValid(active_profile_)) {
    resetToDefaults();
    return false;
  }

  PersistedCalibrationBlock block{};
  block.magic = CALIBRATION_STORAGE_MAGIC;
  block.profile = active_profile_;
  block.use_defaults_override = use_defaults_override_ ? 1U : 0U;
  block.checksum = computeChecksum(block);
  EEPROM.put(kCalibrationStorageAddress, block);
  return true;
}

bool CalibrationStore::setActiveProfile(const CalibrationProfile& profile) {
  if (!isProfileValid(profile)) {
    return false;
  }

  active_profile_ = profile;
  return save();
}

bool CalibrationStore::setDefaultsOverrideEnabled(bool enabled) {
  use_defaults_override_ = enabled;
  return save();
}

void CalibrationStore::resetToDefaults() {
  active_profile_ = defaultCalibrationProfile();
  use_defaults_override_ = false;
  save();
}

const CalibrationProfile& CalibrationStore::active() const {
  if (use_defaults_override_) {
    static const CalibrationProfile kDefaultProfile = defaultCalibrationProfile();
    return kDefaultProfile;
  }
  return active_profile_;
}

bool CalibrationStore::defaultsOverrideEnabled() const { return use_defaults_override_; }

uint16_t CalibrationStore::computeChecksum(const PersistedCalibrationBlock& block) {
  PersistedCalibrationBlock copy = block;
  copy.checksum = 0U;
  const uint8_t* raw = reinterpret_cast<const uint8_t*>(&copy);
  uint16_t checksum = 0U;
  for (size_t i = 0; i < sizeof(copy); ++i) {
    checksum = static_cast<uint16_t>((checksum << 1U) ^ raw[i]);
  }
  return checksum;
}

bool CalibrationStore::isProfileValid(const CalibrationProfile& profile) {
  return profile.flow_pulses_per_liter > 0.0f &&
         profile.wheel_circumference_m > 0.0f &&
         profile.wheel_pulses_per_rev > 0.0f;
}

CalibrationStore& calibrationStore() { return g_calibration_store; }

float activeFlowPulsesPerLiter() { return calibrationStore().active().flow_pulses_per_liter; }

float activeWheelCircumferenceM() { return calibrationStore().active().wheel_circumference_m; }

float activeWheelPulsesPerRev() { return calibrationStore().active().wheel_pulses_per_rev; }

}  // namespace spray
