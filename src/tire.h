#pragma once

#include <cstdint>

namespace toyota_86_tire_pressure_display::tire {

enum class Tire : std::uint8_t {
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3,
};

}
