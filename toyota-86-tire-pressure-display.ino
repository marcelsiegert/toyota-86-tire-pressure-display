#include <Arduino_CAN.h>

#include "config.h"
#include "src/can_protocol.h"
#include "src/display.h"

namespace tpd = toyota_86_tire_pressure_display;

void setup()
{
    tpd::display::init();

    if (!tpd::can_protocol::init()) {
        tpd::display::showError("Error initializing CAN");
        for (;;)
            ;
    }
}

void loop()
{
}
