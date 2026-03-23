#include "interfaces.h"

#include "config.h"

namespace spray {

PumpControl::PumpControl(PwmOutputAdapter& pwm_output) : pwm_output_(pwm_output) {}

void PumpControl::begin() { pwm_output_.begin(); }

void PumpControl::startPWM() { pwm_output_.write(PWM_MIN); }

void PumpControl::setDutyCycle(uint8_t duty) {
  if (duty > PWM_MAX) {
    duty = PWM_MAX;
  }
  pwm_output_.write(duty);
}

}  // namespace spray
