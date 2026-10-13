#include <Arduino_CAN.h>

#include "config.h"
#include "src/can_protocol.h"
#include "src/display.h"

void setup()
{
    toyota_86_tire_pressure_display::display::init();

    if (!toyota_86_tire_pressure_display::can_protocol::init()) {
        toyota_86_tire_pressure_display::display::showError("Error initializing CAN");
        for (;;)
            ;
    }
}

void loop()
{
}
