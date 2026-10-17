#include "body_status.h"

#include <Arduino_CAN.h>

namespace toyota_86_tire_pressure_display::body_status {

bool isHeadlightsOn(const CanMsg& frame)
{
    return (frame.data[3] & 0x08) != 0;
}

}
