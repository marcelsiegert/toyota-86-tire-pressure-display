#pragma once

#include "tire_reading.h"

namespace toyota_86_tire_pressure_display::display {

// Initializes, configures, and clears the display.
void init();

// Draws the readings of all four tires on the display.
void update(const tire_reading::TireReadings& tire_readings);

// Shows a basic error message on the display.
void showError(const char* message);

}
