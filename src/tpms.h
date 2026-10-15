#pragma once

#include <Arduino_CAN.h>
#include <optional>

#include "tire_reading.h"

namespace toyota_86_tire_pressure_display::tpms {

// Starts a TPMS refresh if none is currently running and the refresh interval
// has passed, and eventually returns a reading set once `handleFrame()` has
// handled all frames required for a refresh. Aborts a refresh if the refresh
// timeout is reached. Needs to be called regularly.
[[nodiscard]] std::optional<tire_reading::TireReadings> poll();

// Processes a received `CanMsg` as part of the current TPMS refresh. Non-TPMS
// frames are silently ignored.
void handleFrame(const CanMsg& frame);

}
