#pragma once

#include <Arduino_CAN.h>

namespace toyota_86_tire_pressure_display::body_status {

// Returns whether the headlights or parking lights are on.
bool isHeadlightsOn(const CanMsg& frame);

}
