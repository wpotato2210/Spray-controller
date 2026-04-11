#ifndef SPRAY_CONTROLLER_DISPLAY_H
#define SPRAY_CONTROLLER_DISPLAY_H

#include <Arduino.h>

namespace spray {

void beginDisplay();
void renderDisplay(float flow_lpm,
                   float speed_kmh,
                   uint8_t pump_duty,
                   bool run_enabled,
                   uint8_t active_sections,
                   uint8_t fault_bits);

}  // namespace spray

#endif  // SPRAY_CONTROLLER_DISPLAY_H
