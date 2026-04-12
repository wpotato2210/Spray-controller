#ifndef SPRAY_CONTROLLER_FAULT_MANAGER_H
#define SPRAY_CONTROLLER_FAULT_MANAGER_H

#include <stdint.h>

#include "protocol.h"

namespace spray {

class FaultManager {
 public:
  FaultManager();

  void begin();
  void update(uint8_t observed_fault_bits);
  uint8_t faultBits() const;
  bool hasFaults() const;
  bool shouldInhibitPump() const;

 private:
  uint8_t latched_fault_bits_;
  uint8_t clear_streak_;
};

}  // namespace spray

#endif  // SPRAY_CONTROLLER_FAULT_MANAGER_H
