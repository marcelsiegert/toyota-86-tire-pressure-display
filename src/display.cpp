#include "display.h"

#include <Adafruit_ST7789.h>

#include "../config.h"

namespace toyota_86_tire_pressure_display::display {

namespace {
    Adafruit_ST7789 lcd(config::pin_lcd_cs, config::pin_lcd_dc, config::pin_lcd_rst);
}

void init()
{
    lcd.init(config::lcd_width, config::lcd_height);
    lcd.enableDisplay(false);
    lcd.fillScreen(ST77XX_BLACK);
    lcd.enableDisplay(true);
    lcd.setRotation(config::lcd_rotation);
    lcd.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
}

void showError(const char* message)
{
    lcd.print(message);
}

}
