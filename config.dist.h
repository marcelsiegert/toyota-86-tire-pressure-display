#pragma once

#include <Arduino.h>

namespace toyota_86_tire_pressure_display::config {

// Display pins.
// `cs` = Chip Select, `dc` = Data/Command, `rst` = Reset
constexpr pin_size_t pin_lcd_cs = 10;
constexpr pin_size_t pin_lcd_dc = 7;
constexpr pin_size_t pin_lcd_rst = 8;

}
