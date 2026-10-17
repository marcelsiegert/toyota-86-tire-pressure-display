#pragma once

#include <Arduino_CAN.h>
#include <array>
#include <cstdint>
#include <optional>

namespace toyota_86_tire_pressure_display::can_protocol {

enum class TpmsFrameType : std::uint8_t {
    Pressures,
    Temperatures,
    Consecutive,
};

// Initializes the CAN controller. Returns whether the initialization succeeded.
[[nodiscard]] bool init();

// Returns whether a `CanMsg` is a TPMS response frame.
bool isTpmsFrame(const CanMsg& frame);

// Returns whether a `CanMsg` is a body status frame.
bool isBodyStatusFrame(const CanMsg& frame);

// Requests the tire pressures. Returns whether the request was sent
// successfully.
[[nodiscard]] bool sendTpmsPressuresRequest();

// Requests the tire temperatures. Returns whether the request was sent
// successfully.
[[nodiscard]] bool sendTpmsTemperaturesRequest();

// Requests the consecutive frames of a TPMS response. Returns whether the flow
// control frame was sent successfully.
[[nodiscard]] bool sendTpmsFlowControlFrame();

// Returns the type of a TPMS response frame, or no value if the frame is not
// part of a TPMS response.
std::optional<TpmsFrameType> getTpmsFrameType(const CanMsg& frame);

// Returns the four raw values of a TPMS response, or no value if at least one
// of the frames is invalid. The caller needs to ensure that the consecutive
// frame belongs to the same response as the first frame.
std::optional<std::array<std::uint8_t, 4>> getTpmsRawValuesFromFrames(const CanMsg& first_frame, const CanMsg& consecutive_frame);

}
