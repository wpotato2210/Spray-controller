#include "fault_manager.h"

#include "config.h"

namespace spray {
namespace {
constexpr uint8_t kClearStreakThresholdCycles = 4U;
constexpr uint8_t kPumpInhibitFaultMask =
    STATUS_FAULT_FLOW_CONFIG | STATUS_FAULT_WHEEL_CONFIG | STATUS_FAULT_PRESSURE_CONFIG;
}  // namespace

FaultManager::FaultManager() : latched_fault_bits_(0U), clear_streak_(0U) {}

void FaultManager::begin() {
  latched_fault_bits_ = 0U;
  clear_streak_ = 0U;
}

void FaultManager::update(uint8_t observed_fault_bits) {
  latched_fault_bits_ |= observed_fault_bits;

  if (observed_fault_bits == 0U) {
    if (clear_streak_ < kClearStreakThresholdCycles) {
      ++clear_streak_;
    }
    if (clear_streak_ >= kClearStreakThresholdCycles) {
      latched_fault_bits_ = 0U;
    }
    return;
  }

  clear_streak_ = 0U;
}

uint8_t FaultManager::faultBits() const { return latched_fault_bits_; }

bool FaultManager::hasFaults() const { return latched_fault_bits_ != 0U; }

bool FaultManager::shouldInhibitPump() const {
  return (latched_fault_bits_ & kPumpInhibitFaultMask) != 0U;
}

}  // namespace spray
