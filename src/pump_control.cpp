#include "interfaces.h"

#include <Arduino.h>

#include "config.h"

namespace spray {

PumpControl::PumpControl(uint8_t pin) : pin_(pin) {}

void PumpControl::begin() { pinMode(pin_, OUTPUT); }

void PumpControl::startPWM() { analogWrite(pin_, PWM_MIN); }

void PumpControl::stopPWM() { analogWrite(pin_, PWM_MIN); }

void PumpControl::setDutyCycle(uint8_t duty) {
  if (duty > PWM_MAX) {
    duty = PWM_MAX;
  }
  analogWrite(pin_, duty);
}

}  // namespace spray
