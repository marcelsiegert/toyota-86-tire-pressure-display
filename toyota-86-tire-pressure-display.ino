#include <Arduino.h>
#include <Arduino_CAN.h>
#include <WDT.h>
#include <optional>

#include "config.h"
#include "src/body_status.h"
#include "src/can_protocol.h"
#include "src/display.h"
#include "src/tire_reading.h"
#include "src/tpms.h"

namespace tpd = toyota_86_tire_pressure_display;

namespace {
unsigned long last_tpms_refresh_success_time = 0;
bool is_stale_tire_readings = true;
}

void setup()
{
    // Maximum timeout of the UNO R4.
    WDT.begin(5'592);
    WDT.refresh();

    tpd::display::init();

    if (!tpd::can_protocol::init()) {
        tpd::display::setNightMode(false);
        tpd::display::showError("Error initializing CAN");
        // Let the watchdog timer trigger a hardware reset.
        while (true) { }
    }

    tpd::display::update(tpd::tire_reading::TireReadings { });
}

void loop()
{
    WDT.refresh();

    while (CAN.available()) {
        const CanMsg frame = CAN.read();

        if (tpd::can_protocol::isTpmsFrame(frame)) {
            tpd::tpms::handleFrame(frame);
        } else if (tpd::can_protocol::isBodyStatusFrame(frame)) {
            tpd::display::setNightMode(tpd::body_status::isHeadlightsOn(frame));
        }
    }

    if (const std::optional<tpd::tire_reading::TireReadings> tire_readings = tpd::tpms::poll()) {
        tpd::display::update(*tire_readings);
        last_tpms_refresh_success_time = millis();
        is_stale_tire_readings = false;
    }

    // Reset display data if readings become stale (e.g., CAN bus disconnected).
    if (!is_stale_tire_readings && millis() - last_tpms_refresh_success_time >= tpd::config::tpms_stale_timeout) {
        tpd::display::update(tpd::tire_reading::TireReadings { });
        is_stale_tire_readings = true;
    }
}
