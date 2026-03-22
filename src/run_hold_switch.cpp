#include "interfaces.h"

#include <Arduino.h>

namespace spray {

RunHoldSwitch::RunHoldSwitch(uint8_t pin) : pin_(pin) { pinMode(pin_, INPUT_PULLUP); }

bool RunHoldSwitch::readRunHold() const {
  return digitalRead(pin_) == LOW;
}

}  // namespace spray
