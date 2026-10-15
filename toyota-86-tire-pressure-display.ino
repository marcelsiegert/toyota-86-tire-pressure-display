#include <Arduino_CAN.h>
#include <optional>

#include "config.h"
#include "src/can_protocol.h"
#include "src/display.h"
#include "src/tire_reading.h"
#include "src/tpms.h"

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
    while (CAN.available()) {
        const CanMsg frame = CAN.read();

        if (tpd::can_protocol::isTpmsFrame(frame)) {
            tpd::tpms::handleFrame(frame);
        }
    }

    if (const std::optional<tpd::tire_reading::TireReadings> tire_readings = tpd::tpms::poll()) {
        // @todo Draw readings on the display.
    }
}
