#ifndef SPRAY_CONTROLLER_PINS_H
#define SPRAY_CONTROLLER_PINS_H

/*
target_policy:
  arduino_uno: supported
  arduino_nano: supported
  arduino_mega: supported
*/

#if defined(ARDUINO_AVR_NANO)
#include "pins_nano.h"
#elif defined(ARDUINO_AVR_UNO)
#include "pins_uno.h"
#elif defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
#include "pins_mega2560.h"
#else
#error "Unsupported board target. Define ARDUINO_AVR_NANO, ARDUINO_AVR_UNO, or ARDUINO_AVR_MEGA2560 and provide a matching include/pins_<board>.h mapping."
#endif

#endif  // SPRAY_CONTROLLER_PINS_H
