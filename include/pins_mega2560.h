#ifndef SPRAY_CONTROLLER_PINS_MEGA2560_H
#define SPRAY_CONTROLLER_PINS_MEGA2560_H

#include <Arduino.h>

#include "config.h"

namespace spray {

constexpr uint8_t PIN_WHEEL_SENSOR = 18U;
constexpr uint8_t PIN_FLOW_SENSOR = 19U;
constexpr uint8_t PIN_PUMP_PWM = 44U;

constexpr uint8_t PIN_BOOM_1 = 22U;
constexpr uint8_t PIN_BOOM_2 = 23U;
constexpr uint8_t PIN_BOOM_3 = 24U;

constexpr uint8_t PIN_LED_SECTION_1 = 25U;

constexpr uint8_t PIN_RUN_HOLD = 26U;
constexpr uint8_t PIN_SECTION_SW_1 = 27U;
constexpr uint8_t PIN_SECTION_SW_2 = 28U;
constexpr uint8_t PIN_SECTION_SW_3 = 29U;
constexpr uint8_t PIN_PRESSURE_SENSOR = A8;

constexpr std::array<SectionDescriptor, SECTION_COUNT> kSectionDescriptors = {{{
    {0U, PIN_BOOM_1, PIN_SECTION_SW_1},
    {1U, PIN_BOOM_2, PIN_SECTION_SW_2},
    {2U, PIN_BOOM_3, PIN_SECTION_SW_3},
}}};

static_assert(kSectionDescriptors.size() == SECTION_COUNT,
              "Section descriptor count must match SECTION_COUNT.");

}  // namespace spray

#endif  // SPRAY_CONTROLLER_PINS_MEGA2560_H
