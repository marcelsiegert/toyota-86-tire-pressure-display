#pragma once

#include <array>
#include <cstdint>

namespace toyota_86_tire_pressure_display::tire_reading {

// Pressure and temperature reading of a single tire.
struct TireReading {
    std::uint8_t raw_pressure = 0;
    std::uint8_t raw_temperature = 0;
};

// Readings of all four tires, indexed by `tire::Tire`.
using TireReadings = std::array<TireReading, 4>;

}
