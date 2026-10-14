#pragma once

#include <Arduino.h>
#include <cstdint>

namespace toyota_86_tire_pressure_display::config {

// Display pins.
// `bl` = Backlight, `cs` = Chip Select, `dc` = Data/Command, `rst` = Reset
constexpr pin_size_t pin_lcd_bl = 9;
constexpr pin_size_t pin_lcd_cs = 10;
constexpr pin_size_t pin_lcd_dc = 7;
constexpr pin_size_t pin_lcd_rst = 8;

// Display dimensions and rotation. Dimensions refer to the display before
// rotation.
constexpr std::uint16_t lcd_width = 172;
constexpr std::uint16_t lcd_height = 320;
constexpr std::uint8_t lcd_rotation = 1;

// Display brightness.
constexpr std::uint8_t lcd_brightness = 255;

}
