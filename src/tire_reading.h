#pragma once

#include <array>
#include <cstdint>
#include <optional>

namespace toyota_86_tire_pressure_display::tire_reading {

namespace {
    // In kPa.
    constexpr std::uint8_t atmospheric_pressure = 100;
}

// Pressure and temperature reading of a single tire.
struct TireReading {
    std::uint8_t raw_pressure = 0;
    std::uint8_t raw_temperature = 0;

    // Gauge pressure in kPa, or no value if the sensor has not sent any data
    // yet.
    std::optional<std::uint16_t> getPressure() const
    {
        if (raw_pressure == 0) {
            return std::nullopt;
        }

        const std::uint16_t absolute_pressure = raw_pressure * 2;

        if (absolute_pressure < atmospheric_pressure) {
            return 0;
        }

        return absolute_pressure - atmospheric_pressure;
    }

    // Temperature in °C, or no value if the sensor has not sent any data yet.
    std::optional<std::int16_t> getTemperature() const
    {
        if (raw_temperature == 0) {
            return std::nullopt;
        }

        return raw_temperature - 40;
    }
};

// Readings of all four tires, indexed by `tire::Tire`.
using TireReadings = std::array<TireReading, 4>;

}
