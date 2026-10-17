#pragma once

#include <Arduino.h>
#include <array>
#include <cstdint>

#include "src/tire.h"

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

// Empty space between the display edges and the content.
constexpr std::uint16_t lcd_margin_top = 6;
constexpr std::uint16_t lcd_margin_right = 6;
constexpr std::uint16_t lcd_margin_bottom = 6;
constexpr std::uint16_t lcd_margin_left = 6;

// Display text sizes.
constexpr std::uint8_t lcd_text_size_pressure = 7;
constexpr std::uint8_t lcd_text_size_temperature = 2;

// Display spacing between the pressure and the temperature, in pixels.
constexpr std::uint16_t lcd_line_spacing = 7;

// Time between TPMS refreshes, in ms.
constexpr std::uint32_t tpms_refresh_interval = 10'000;

// Time after which a pending refresh is aborted, in ms. Should be less than
// `tpms_refresh_interval`.
constexpr std::uint32_t tpms_refresh_timeout = 2'000;

// Which tire each of the four values in the TPMS response belongs to. If the
// sensor IDs were programmed clockwise, starting with the front left tire, the
// default order is already correct.
constexpr std::array<tire::Tire, 4> tire_can_order = {
    tire::Tire::FrontLeft,
    tire::Tire::FrontRight,
    tire::Tire::RearRight,
    tire::Tire::RearLeft,
};

}
