#ifndef SPRAY_CONTROLLER_INTERRUPT_GUARDS_H
#define SPRAY_CONTROLLER_INTERRUPT_GUARDS_H

#include <Arduino.h>

#include "pins.h"

namespace spray {

#if defined(ARDUINO_AVR_NANO) || defined(ARDUINO_AVR_UNO)
static_assert((PIN_FLOW_SENSOR == 2U) || (PIN_FLOW_SENSOR == 3U),
              "PIN_FLOW_SENSOR must be interrupt-capable on AVR UNO/NANO (D2 or D3).");
static_assert((PIN_WHEEL_SENSOR == 2U) || (PIN_WHEEL_SENSOR == 3U),
              "PIN_WHEEL_SENSOR must be interrupt-capable on AVR UNO/NANO (D2 or D3).");
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
constexpr bool isMegaExternalInterruptPin(uint8_t pin) {
  return (pin == 2U) || (pin == 3U) || (pin == 18U) || (pin == 19U) || (pin == 20U) ||
         (pin == 21U);
}
static_assert(isMegaExternalInterruptPin(PIN_FLOW_SENSOR),
              "PIN_FLOW_SENSOR must be Mega external interrupt-capable (D2, D3, D18, D19, D20, D21).");
static_assert(isMegaExternalInterruptPin(PIN_WHEEL_SENSOR),
              "PIN_WHEEL_SENSOR must be Mega external interrupt-capable (D2, D3, D18, D19, D20, D21).");
#endif

inline bool interruptPinsValid() {
  return (digitalPinToInterrupt(PIN_FLOW_SENSOR) != NOT_AN_INTERRUPT) &&
         (digitalPinToInterrupt(PIN_WHEEL_SENSOR) != NOT_AN_INTERRUPT);
}

}  // namespace spray

#endif  // SPRAY_CONTROLLER_INTERRUPT_GUARDS_H
