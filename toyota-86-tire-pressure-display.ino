#include <Adafruit_ST7789.h>
#include <Arduino_CAN.h>

#include "config.h"

static Adafruit_ST7789 LCD(
    toyota_86_tire_pressure_display::config::pin_lcd_cs,
    toyota_86_tire_pressure_display::config::pin_lcd_dc,
    toyota_86_tire_pressure_display::config::pin_lcd_rst);

void setup()
{
    LCD.init(172, 320);
    LCD.enableDisplay(false);
    LCD.fillScreen(ST77XX_BLACK);
    LCD.enableDisplay(true);

    CAN.setFilterMask_Standard(0x1FFC0000);
    for (size_t mailbox = 0; mailbox < R7FA4M1_CAN::CAN_MAX_NO_STANDARD_MAILBOXES; mailbox++) {
        CAN.setFilterId_Standard(mailbox, 0x758);
    }
    if (!CAN.begin(CanBitRate::BR_500k)) {
        LCD.print("Error initializing CAN");
        for (;;)
            ;
    }
}

void loop()
{
}
