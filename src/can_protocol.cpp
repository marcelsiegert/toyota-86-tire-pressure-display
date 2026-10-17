#include "can_protocol.h"

#include <Arduino_CAN.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace toyota_86_tire_pressure_display::can_protocol {

namespace {
    constexpr std::uint32_t tpms_request_id = 0x750;
    constexpr std::uint32_t tpms_response_id = 0x758;
    constexpr std::uint8_t tpms_extended_id = 0x2A;
    constexpr std::uint8_t tpms_uds_service_id = 0x21;
    constexpr std::uint32_t body_status_id = 0x375;
    constexpr std::uint8_t uds_positive_response_offset = 0x40;
    constexpr std::uint8_t tpms_tire_pressures_pid = 0x30;
    constexpr std::uint8_t tpms_tire_temperatures_pid = 0x16;

    enum class IsoTpFrameType : std::uint8_t {
        Single = 0,
        First = 1,
        Consecutive = 2,
        FlowControl = 3,
    };

    enum class IsoTpFlowControlFlag : std::uint8_t {
        ContinueToSend = 0,
        Wait = 1,
        Overflow = 2,
    };
}

bool init()
{
    // Only receive TPMS responses and body status frames.
    CAN.setFilterMask_Standard(0x7FF << 18);
    for (std::size_t mailbox = 0; mailbox < R7FA4M1_CAN::CAN_MAX_NO_STANDARD_MAILBOXES; mailbox++) {
        CAN.setFilterId_Standard(mailbox, mailbox % 2 == 0 ? tpms_response_id : body_status_id);
    }
    return CAN.begin(CanBitRate::BR_500k);
}

bool isTpmsFrame(const CanMsg& frame)
{
    return frame.id == tpms_response_id
        && frame.data_length == 8
        && frame.data[0] == tpms_extended_id;
}

bool isBodyStatusFrame(const CanMsg& frame)
{
    return frame.id == body_status_id
        && frame.data_length == 8;
}

bool sendTpmsPressuresRequest()
{
    constexpr std::uint8_t length = 2;
    constexpr std::uint8_t frame_data[8] = {
        tpms_extended_id,
        (static_cast<std::uint8_t>(IsoTpFrameType::Single) << 4) | length,
        tpms_uds_service_id,
        tpms_tire_pressures_pid,
    };

    return CAN.write(CanMsg(CanStandardId(tpms_request_id), sizeof(frame_data), frame_data)) > 0;
}

bool sendTpmsTemperaturesRequest()
{
    constexpr std::uint8_t length = 2;
    constexpr std::uint8_t frame_data[8] = {
        tpms_extended_id,
        (static_cast<std::uint8_t>(IsoTpFrameType::Single) << 4) | length,
        tpms_uds_service_id,
        tpms_tire_temperatures_pid,
    };

    return CAN.write(CanMsg(CanStandardId(tpms_request_id), sizeof(frame_data), frame_data)) > 0;
}

bool sendTpmsFlowControlFrame()
{
    constexpr std::uint8_t frame_data[8] = {
        tpms_extended_id,
        (static_cast<std::uint8_t>(IsoTpFrameType::FlowControl) << 4) | static_cast<std::uint8_t>(IsoTpFlowControlFlag::ContinueToSend),
    };

    return CAN.write(CanMsg(CanStandardId(tpms_request_id), sizeof(frame_data), frame_data)) > 0;
}

std::optional<TpmsFrameType> getTpmsFrameType(const CanMsg& frame)
{
    if (!isTpmsFrame(frame)) {
        return std::nullopt;
    }

    const auto frame_type = static_cast<IsoTpFrameType>(frame.data[1] >> 4);

    if (frame_type == IsoTpFrameType::First && frame.data[3] == (tpms_uds_service_id + uds_positive_response_offset)) {
        if (frame.data[4] == tpms_tire_pressures_pid) {
            return TpmsFrameType::Pressures;
        }
        if (frame.data[4] == tpms_tire_temperatures_pid) {
            return TpmsFrameType::Temperatures;
        }
    } else if (frame_type == IsoTpFrameType::Consecutive && (frame.data[1] & 0x0F) == 1) {
        return TpmsFrameType::Consecutive;
    }

    return std::nullopt;
}

std::optional<std::array<std::uint8_t, 4>> getTpmsRawValuesFromFrames(const CanMsg& first_frame, const CanMsg& consecutive_frame)
{
    const std::optional<TpmsFrameType> first_frame_type = getTpmsFrameType(first_frame);
    const std::optional<TpmsFrameType> consecutive_frame_type = getTpmsFrameType(consecutive_frame);

    if (first_frame_type != TpmsFrameType::Pressures && first_frame_type != TpmsFrameType::Temperatures) {
        return std::nullopt;
    }
    if (consecutive_frame_type != TpmsFrameType::Consecutive) {
        return std::nullopt;
    }

    return std::array<std::uint8_t, 4> {
        first_frame.data[5],
        first_frame.data[6],
        first_frame.data[7],
        consecutive_frame.data[2],
    };
}

}
