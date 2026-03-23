#ifndef SPRAY_CONTROLLER_PINS_UNO_H
#define SPRAY_CONTROLLER_PINS_UNO_H

#include <Arduino.h>

#include "config.h"

namespace spray {

constexpr uint8_t PIN_WHEEL_SENSOR = 2U;
constexpr uint8_t PIN_FLOW_SENSOR = 3U;
constexpr uint8_t PIN_PUMP_PWM = 11U;

constexpr uint8_t PIN_BOOM_1 = 12U;
constexpr uint8_t PIN_BOOM_2 = 13U;
constexpr uint8_t PIN_BOOM_3 = A1;

constexpr uint8_t PIN_LED_SECTION_1 = A2;

constexpr uint8_t PIN_RUN_HOLD = A3;
constexpr uint8_t PIN_SECTION_SW_1 = A4;
constexpr uint8_t PIN_SECTION_SW_2 = A5;
constexpr uint8_t PIN_SECTION_SW_3 = 4U;
constexpr uint8_t PIN_PRESSURE_SENSOR = A0;

constexpr std::array<SectionDescriptor, SECTION_COUNT> kSectionDescriptors = {{{
    {0U, PIN_BOOM_1, PIN_SECTION_SW_1},
    {1U, PIN_BOOM_2, PIN_SECTION_SW_2},
    {2U, PIN_BOOM_3, PIN_SECTION_SW_3},
}}};

static_assert(kSectionDescriptors.size() == SECTION_COUNT,
              "Section descriptor count must match SECTION_COUNT.");

}  // namespace spray

#endif  // SPRAY_CONTROLLER_PINS_UNO_H
