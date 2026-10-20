#include "display.h"

#include <Adafruit_GFX.h>
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
    std::uint8_t current_brightness = 0;

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

    void setBrightness(std::uint8_t value)
    {
        if (value != current_brightness) {
            analogWrite(config::pin_lcd_bl, value);
            current_brightness = value;
        }
    }
}

void init()
{
    pinMode(config::pin_lcd_bl, PinMode::OUTPUT);
    analogWrite(config::pin_lcd_bl, current_brightness);

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
        // The space keeps the line's height when there is no temperature.
        char temperature_text[4] = " ";

        if (const std::optional<std::uint16_t> pressure = reading.getPressure()) {
            std::snprintf(pressure_text, sizeof(pressure_text), "%03u", *pressure);

            if (const std::optional<std::int16_t> temperature = reading.getTemperature()) {
                std::snprintf(temperature_text, sizeof(temperature_text), "%d", *temperature);
            }
        }

        const TextBounds pressure_bounds = getTextBounds(pressure_text, config::lcd_text_size_pressure);
        const TextBounds temperature_bounds = getTextBounds(temperature_text, config::lcd_text_size_temperature);

        GFXcanvas1 canvas(
            pressure_bounds.width,
            config::lcd_line_spacing + pressure_bounds.height + temperature_bounds.height);
        canvas.setTextWrap(false);

        canvas.setTextSize(config::lcd_text_size_pressure);
        canvas.setCursor(0, 0);
        canvas.print(pressure_text);

        canvas.setTextSize(config::lcd_text_size_temperature);
        canvas.setCursor(
            (pressure_bounds.width / 2) - (temperature_bounds.width / 2),
            config::lcd_line_spacing + pressure_bounds.height);
        canvas.print(temperature_text);

        std::int16_t canvas_x = config::lcd_margin_left;
        if (tire_position == tire::Tire::FrontRight || tire_position == tire::Tire::RearRight) {
            canvas_x = lcd.width() - canvas.width() - config::lcd_margin_right;
        }

        std::int16_t canvas_y = config::lcd_margin_top;
        if (tire_position == tire::Tire::RearLeft || tire_position == tire::Tire::RearRight) {
            canvas_y = lcd.height() - canvas.height() - config::lcd_margin_bottom;
        }

        lcd.drawBitmap(canvas_x, canvas_y, canvas.getBuffer(), canvas.width(), canvas.height(), ST77XX_WHITE, ST77XX_BLACK);
    }
}

void setNightMode(bool enabled)
{
    setBrightness(enabled ? config::lcd_brightness_nighttime : config::lcd_brightness_daytime);
}

void showError(const char* message)
{
    lcd.setTextSize(1);
    lcd.setCursor(config::lcd_margin_left, config::lcd_margin_top);
    lcd.print(message);
}

}
