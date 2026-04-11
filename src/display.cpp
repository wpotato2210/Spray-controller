#include "display.h"

#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
#include <U8g2lib.h>

#include "pins.h"

namespace spray {
namespace {
U8G2_ST7920_128X64_F_SW_SPI g_display(U8G2_R0, PIN_LCD_SCLK, PIN_LCD_MOSI, PIN_LCD_CS, PIN_LCD_RST);

void drawLabelValue(uint8_t y, const char* label, const char* value) {
  g_display.drawStr(0, y, label);
  g_display.drawStr(58, y, value);
}
}  // namespace

void beginDisplay() {
  g_display.begin();
  g_display.setContrast(30);
  g_display.clearBuffer();
  g_display.setFont(u8g2_font_6x10_tr);
  g_display.drawStr(0, 12, "Spray Controller");
  g_display.drawStr(0, 26, "ST7920 ready");
  g_display.sendBuffer();
}

void renderDisplay(float flow_lpm,
                   float speed_kmh,
                   uint8_t pump_duty,
                   bool run_enabled,
                   uint8_t active_sections,
                   uint8_t fault_bits) {
  char line0[20];
  char line1[20];
  char line2[20];
  char line3[20];

  dtostrf(flow_lpm, 6, 2, line0);
  dtostrf(speed_kmh, 6, 2, line1);
  snprintf(line2, sizeof(line2), "%3u", pump_duty);
  snprintf(line3, sizeof(line3), "%u F:%u", active_sections, fault_bits);

  g_display.clearBuffer();
  g_display.setFont(u8g2_font_6x10_tr);
  drawLabelValue(12, "FLOW LPM:", line0);
  drawLabelValue(24, "SPD KMH:", line1);
  drawLabelValue(36, "PUMP PWM:", line2);
  drawLabelValue(48, run_enabled ? "RUN/SECT:" : "HOLD/SECT:", line3);
  g_display.sendBuffer();
}

}  // namespace spray
#else
namespace spray {

void beginDisplay() {}

void renderDisplay(float,
                   float,
                   uint8_t,
                   bool,
                   uint8_t,
                   uint8_t) {}

}  // namespace spray
#endif
