#ifndef SPRAY_CONTROLLER_PINS_H
#define SPRAY_CONTROLLER_PINS_H

#if defined(ARDUINO_AVR_NANO)
#include "pins_nano.h"
#elif defined(ARDUINO_AVR_UNO)
#include "pins_uno.h"
#else
#error "Unsupported board target. Define ARDUINO_AVR_NANO or ARDUINO_AVR_UNO and provide a matching include/pins_<board>.h mapping."
#endif

#endif  // SPRAY_CONTROLLER_PINS_H
