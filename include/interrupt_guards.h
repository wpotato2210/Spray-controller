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
#endif

inline bool interruptPinsValid() {
  return (digitalPinToInterrupt(PIN_FLOW_SENSOR) != NOT_AN_INTERRUPT) &&
         (digitalPinToInterrupt(PIN_WHEEL_SENSOR) != NOT_AN_INTERRUPT);
}

}  // namespace spray

#endif  // SPRAY_CONTROLLER_INTERRUPT_GUARDS_H
