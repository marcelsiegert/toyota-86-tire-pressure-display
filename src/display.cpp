#include "display.h"

#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <optional>

#include "../config.h"
#include "tire.h"
#include "tire_reading.h"

namespace toyota_86_tire_pressure_display::display {

namespace {
    struct TextBounds {
        std::uint16_t width;
        std::uint16_t height;
    };

    Adafruit_ST7789 lcd(config::pin_lcd_cs, config::pin_lcd_dc, config::pin_lcd_rst);

    // Returns the bounds of a text at the given text size, without the spacing
    // the font appends to the bottom and right of each character. Only valid
    // for the standard monospace font.
    TextBounds getTextBounds(const char* text, std::uint8_t text_size)
    {
        TextBounds bounds;
        std::int16_t x1, y1;

        lcd.setTextSize(text_size);
        lcd.getTextBounds(text, 0, 0, &x1, &y1, &bounds.width, &bounds.height);
        bounds.width -= text_size;
        bounds.height -= text_size;

        return bounds;
    }
}

void init()
{
    pinMode(config::pin_lcd_bl, PinMode::OUTPUT);
    analogWrite(config::pin_lcd_bl, config::lcd_brightness);

    lcd.init(config::lcd_width, config::lcd_height);
    lcd.enableDisplay(false);
    lcd.fillScreen(ST77XX_BLACK);
    lcd.enableDisplay(true);
    lcd.setRotation(config::lcd_rotation);
    lcd.setTextWrap(false);
    lcd.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
}

void update(const tire_reading::TireReadings& tire_readings)
{
    for (const auto tire_position : { tire::Tire::FrontLeft, tire::Tire::FrontRight, tire::Tire::RearRight, tire::Tire::RearLeft }) {
        const tire_reading::TireReading& reading = tire_readings[static_cast<std::size_t>(tire_position)];

        char pressure_text[4] = "---";
        char temperature_text[6] = "     ";

        if (const std::optional<std::uint16_t> pressure = reading.getPressure()) {
            std::snprintf(pressure_text, sizeof(pressure_text), "%03u", *pressure);

            if (const std::optional<std::int16_t> temperature = reading.getTemperature()) {
                // Spaces around the temperature overwrite previous, possibly
                // longer, temperature values.
                std::snprintf(temperature_text, sizeof(temperature_text), " %d ", *temperature);
            }
        }

        const TextBounds pressure_bounds = getTextBounds(pressure_text, config::lcd_text_size_pressure);
        const TextBounds temperature_bounds = getTextBounds(temperature_text, config::lcd_text_size_temperature);

        std::int16_t pressure_x = config::lcd_margin_left;
        std::int16_t temperature_x = config::lcd_margin_left + (pressure_bounds.width / 2) - (temperature_bounds.width / 2);
        if (tire_position == tire::Tire::FrontRight || tire_position == tire::Tire::RearRight) {
            pressure_x = lcd.width() - config::lcd_margin_right - pressure_bounds.width;
            temperature_x = lcd.width() - config::lcd_margin_right - (pressure_bounds.width / 2) - (temperature_bounds.width / 2);
        }

        std::int16_t pressure_y = config::lcd_margin_top;
        std::int16_t temperature_y = config::lcd_margin_top + config::lcd_line_spacing + pressure_bounds.height;
        if (tire_position == tire::Tire::RearLeft || tire_position == tire::Tire::RearRight) {
            pressure_y = lcd.height() - config::lcd_margin_bottom - config::lcd_line_spacing - pressure_bounds.height - temperature_bounds.height;
            temperature_y = lcd.height() - config::lcd_margin_bottom - temperature_bounds.height;
        }

        lcd.setTextSize(config::lcd_text_size_pressure);
        lcd.setCursor(pressure_x, pressure_y);
        lcd.print(pressure_text);

        lcd.setTextSize(config::lcd_text_size_temperature);
        lcd.setCursor(temperature_x, temperature_y);
        lcd.print(temperature_text);
    }
}

void showError(const char* message)
{
    lcd.setTextSize(1);
    lcd.setCursor(config::lcd_margin_left, config::lcd_margin_top);
    lcd.print(message);
}

}
