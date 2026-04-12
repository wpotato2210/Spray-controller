#ifndef SPRAY_CONTROLLER_PIN_POLICY_GUARDS_H
#define SPRAY_CONTROLLER_PIN_POLICY_GUARDS_H

#include <array>
#include <stdint.h>

#include "pin_map_lcd.h"
#include "pins.h"

namespace spray {
namespace detail {

constexpr uint8_t kPinUnassigned = 0xFFU;

template <size_t N>
constexpr bool hasUniqueAssignedPins(const std::array<uint8_t, N>& pins) {
  for (size_t i = 0; i < N; ++i) {
    if (pins[i] == kPinUnassigned) {
      continue;
    }
    for (size_t j = i + 1; j < N; ++j) {
      if (pins[i] == pins[j]) {
        return false;
      }
    }
  }
  return true;
}

template <size_t N, size_t M>
constexpr bool containsAnyAssignedPin(const std::array<uint8_t, N>& pins,
                                      const std::array<uint8_t, M>& reserved) {
  for (size_t i = 0; i < N; ++i) {
    if (pins[i] == kPinUnassigned) {
      continue;
    }
    for (size_t j = 0; j < M; ++j) {
      if (pins[i] == reserved[j]) {
        return true;
      }
    }
  }
  return false;
}

constexpr std::array<uint8_t, 16> kAssignedRolePins = {{
    PIN_WHEEL_SENSOR,
    PIN_FLOW_SENSOR,
    PIN_PUMP_PWM,
    PIN_SECTION_RELAY_1,
    PIN_SECTION_RELAY_2,
    PIN_SECTION_RELAY_3,
    PIN_LED_SECTION_1,
    PIN_RUN_HOLD,
    PIN_SECTION_SW_1,
    PIN_SECTION_SW_2,
    PIN_SECTION_SW_3,
    PIN_ENCODER_CLK,
    PIN_ENCODER_DT,
    PIN_ENCODER_SW,
    PIN_PRESSURE_SENSOR,
    PIN_BTN_SELECT,
}};

constexpr std::array<uint8_t, 5> kUiPins = {{
    PIN_BTN_UP,
    PIN_BTN_DOWN,
    PIN_BTN_CAL,
    PIN_BTN_SELECT,
    PIN_BTN_AUTO_MANUAL,
}};

}  // namespace detail

static_assert(detail::hasUniqueAssignedPins(detail::kAssignedRolePins),
              "Duplicate assigned pin role detected; each enabled role must use a unique pin.");
static_assert(detail::hasUniqueAssignedPins(detail::kUiPins),
              "UI pins must be unique when allocated.");

#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
constexpr std::array<uint8_t, 4> kReservedLcdPins = {{LCD_CLK, LCD_MOSI, LCD_CS, LCD_RESET}};
static_assert(!detail::containsAnyAssignedPin(detail::kUiPins, kReservedLcdPins),
              "UI pins may not overlap reserved Mega LCD/SPI pins (52, 51, 53, 49).");
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
constexpr std::array<uint8_t, 4> kReservedLcdPins = {{LCD_CLK, LCD_MOSI, LCD_CS, LCD_RESET}};
static_assert(!detail::containsAnyAssignedPin(detail::kUiPins, kReservedLcdPins),
              "UI pins may not overlap reserved Uno/Nano LCD/SPI pins (13, 11, 10, 8).");
#endif

}  // namespace spray

#endif  // SPRAY_CONTROLLER_PIN_POLICY_GUARDS_H
