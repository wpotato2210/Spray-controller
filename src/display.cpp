#include "display.h"

#include <stdio.h>

namespace spray {
namespace {

#if !defined(DISPLAY_BACKEND_ST7920_SPI)
#define DISPLAY_BACKEND_ST7920_SPI 1
#endif

#if !defined(DISPLAY_BACKEND_NONE)
#define DISPLAY_BACKEND_NONE 0
#endif

#ifndef DISPLAY_BACKEND
#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
#define DISPLAY_BACKEND DISPLAY_BACKEND_ST7920_SPI
#else
#define DISPLAY_BACKEND DISPLAY_BACKEND_NONE
#endif
#endif

#if (DISPLAY_BACKEND == DISPLAY_BACKEND_ST7920_SPI)
#include <U8g2lib.h>

#include "pin_map_lcd.h"

#ifndef LCD_CS
#error "LCD pin mapping not defined for this board"
#endif

class St7920SpiDisplayAdapter final : public DisplayAdapter {
 public:
  void begin() override {
    display_.begin();
    display_.setContrast(30);
    display_.firstPage();
    do {
      display_.setFont(u8g2_font_6x10_tr);
      display_.drawStr(0, 12, "Spray Controller");
      display_.drawStr(0, 26, "ST7920 ready");
    } while (display_.nextPage());
  }

  void render(const DisplayFrame& frame) override {
    char line0[20];
    char line1[20];
    char line2[20];
    char line3[20];

    dtostrf(frame.flow_lpm, 6, 2, line0);
    dtostrf(frame.speed_kmh, 6, 2, line1);
    snprintf(line2, sizeof(line2), "%3u", frame.pump_duty);
    snprintf(line3, sizeof(line3), "%u F:%u", frame.active_sections, frame.fault_bits);

    display_.firstPage();
    do {
      display_.setFont(u8g2_font_6x10_tr);
      drawLabelValue(12, "FLOW LPM:", line0);
      drawLabelValue(24, "SPD KMH:", line1);
      drawLabelValue(36, "PUMP PWM:", line2);
      drawLabelValue(48, frame.run_enabled ? "RUN/SECT:" : "HOLD/SECT:", line3);
    } while (display_.nextPage());
  }

 private:
  void drawLabelValue(uint8_t y, const char* label, const char* value) {
    display_.drawStr(0, y, label);
    display_.drawStr(58, y, value);
  }

  U8G2_ST7920_128X64_1_HW_SPI display_{U8G2_R0, LCD_CS, LCD_RESET};
};

DisplayAdapter& displayAdapter() {
  static St7920SpiDisplayAdapter adapter;
  return adapter;
}

#elif (DISPLAY_BACKEND == DISPLAY_BACKEND_NONE)

class NullDisplayAdapter final : public DisplayAdapter {
 public:
  void begin() override {}
  void render(const DisplayFrame&) override {}
};

DisplayAdapter& displayAdapter() {
  static NullDisplayAdapter adapter;
  return adapter;
}

#else
#error "Unsupported DISPLAY_BACKEND selection"
#endif

}  // namespace

void beginDisplay() { displayAdapter().begin(); }

void renderDisplay(float flow_lpm,
                   float speed_kmh,
                   uint8_t pump_duty,
                   bool run_enabled,
                   uint8_t active_sections,
                   uint8_t fault_bits) {
  displayAdapter().render({flow_lpm, speed_kmh, pump_duty, run_enabled, active_sections, fault_bits});
}

}  // namespace spray
