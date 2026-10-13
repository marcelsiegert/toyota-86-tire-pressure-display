#include "can_protocol.h"

#include <Arduino_CAN.h>
#include <cstddef>
#include <cstdint>

namespace toyota_86_tire_pressure_display::can_protocol {

namespace {
    constexpr std::uint32_t tpms_response_id = 0x758;
}

bool init()
{
    // Only receive TPMS responses.
    CAN.setFilterMask_Standard(0x7FF << 18);
    for (std::size_t mailbox = 0; mailbox < R7FA4M1_CAN::CAN_MAX_NO_STANDARD_MAILBOXES; mailbox++) {
        CAN.setFilterId_Standard(mailbox, tpms_response_id);
    }
    return CAN.begin(CanBitRate::BR_500k);
}

}
