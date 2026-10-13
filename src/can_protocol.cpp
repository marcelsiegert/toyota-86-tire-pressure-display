#include "can_protocol.h"

#include <Arduino_CAN.h>
#include <cstddef>

namespace toyota_86_tire_pressure_display::can_protocol {

bool init()
{
    // Only receive TPMS responses.
    CAN.setFilterMask_Standard(0x1FFC0000);
    for (std::size_t mailbox = 0; mailbox < R7FA4M1_CAN::CAN_MAX_NO_STANDARD_MAILBOXES; mailbox++) {
        CAN.setFilterId_Standard(mailbox, 0x758);
    }
    return CAN.begin(CanBitRate::BR_500k);
}

}
