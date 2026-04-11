#ifndef SPRAY_CONTROLLER_PIN_MAP_LCD_H
#define SPRAY_CONTROLLER_PIN_MAP_LCD_H

#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
#define LCD_CLK 52
#define LCD_MOSI 51
#define LCD_CS 53
#define LCD_RESET 49
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
#define LCD_CLK 13
#define LCD_MOSI 11
#define LCD_CS 10
#define LCD_RESET 8
#else
#error "Unsupported board for LCD"
#endif

#endif  // SPRAY_CONTROLLER_PIN_MAP_LCD_H
