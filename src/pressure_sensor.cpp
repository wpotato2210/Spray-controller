#include "interfaces.h"

#include <Arduino.h>

#include "config.h"

namespace spray {

PressureSensor::PressureSensor(uint8_t pin) : pin_(pin), config_fault_active_(false) {}

void PressureSensor::begin() { pinMode(pin_, INPUT); }

float PressureSensor::readPressure() {
  const int raw = analogRead(pin_);
  config_fault_active_ = PRESSURE_SENSOR_MAX_KPA < PRESSURE_SENSOR_MIN_KPA;
  if (config_fault_active_) {
    return PRESSURE_SENSOR_MIN_KPA;
  }

  const float normalized = static_cast<float>(raw) / 1023.0f;
  float pressure_kpa =
      PRESSURE_SENSOR_MIN_KPA +
      (normalized * (PRESSURE_SENSOR_MAX_KPA - PRESSURE_SENSOR_MIN_KPA));
  if (pressure_kpa < PRESSURE_SENSOR_MIN_KPA) {
    pressure_kpa = PRESSURE_SENSOR_MIN_KPA;
  }
  if (pressure_kpa > PRESSURE_SENSOR_MAX_KPA) {
    pressure_kpa = PRESSURE_SENSOR_MAX_KPA;
  }
  return pressure_kpa;
}

void PressureSensor::reset() { config_fault_active_ = false; }

bool PressureSensor::isConfigFaultActive() const { return config_fault_active_; }

}  // namespace spray
