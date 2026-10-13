#include "display.h"

#include <Adafruit_ST7789.h>

#include "../config.h"

namespace toyota_86_tire_pressure_display::display {

namespace {
    Adafruit_ST7789 lcd(config::pin_lcd_cs, config::pin_lcd_dc, config::pin_lcd_rst);
}

void init()
{
    lcd.init(172, 320);
    lcd.enableDisplay(false);
    lcd.fillScreen(ST77XX_BLACK);
    lcd.enableDisplay(true);
}

void showError(const char* message)
{
    lcd.print(message);
}

}
