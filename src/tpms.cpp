#include "tpms.h"

#include <Arduino.h>
#include <Arduino_CAN.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "../config.h"
#include "can_protocol.h"
#include "tire_reading.h"

namespace toyota_86_tire_pressure_display::tpms {

namespace {
    enum class RefreshState : std::uint8_t {
        Idle,
        AwaitPressuresResponse,
        AwaitPressuresConsecutiveResponse,
        AwaitTemperaturesResponse,
        AwaitTemperaturesConsecutiveResponse,
        Complete,
    };

    RefreshState refresh_state = RefreshState::Idle;
    tire_reading::TireReadings readings_buffer { };
    CanMsg first_frame_buffer { };
    unsigned long last_refresh_start_time = 0;
}

std::optional<tire_reading::TireReadings> poll()
{
    switch (refresh_state) {
    case RefreshState::Idle:
        if (millis() - last_refresh_start_time >= config::tpms_refresh_interval) {
            last_refresh_start_time = millis();
            readings_buffer = { };

            if (can_protocol::sendTpmsPressuresRequest()) {
                refresh_state = RefreshState::AwaitPressuresResponse;
            }
        }
        break;

    case RefreshState::Complete:
        refresh_state = RefreshState::Idle;
        return readings_buffer;

    default:
        if (millis() - last_refresh_start_time >= config::tpms_refresh_timeout) {
            refresh_state = RefreshState::Idle;
        }
        break;
    }

    return std::nullopt;
}

void handleFrame(const CanMsg& frame)
{
    const std::optional<can_protocol::TpmsFrameType> type = can_protocol::getTpmsFrameType(frame);
    if (!type) {
        return;
    }

    switch (refresh_state) {
    case RefreshState::AwaitPressuresResponse:
        if (*type != can_protocol::TpmsFrameType::Pressures) {
            break;
        }

        first_frame_buffer = frame;

        if (!can_protocol::sendTpmsFlowControlFrame()) {
            refresh_state = RefreshState::Idle;
            break;
        }

        refresh_state = RefreshState::AwaitPressuresConsecutiveResponse;
        break;

    case RefreshState::AwaitPressuresConsecutiveResponse: {
        if (*type != can_protocol::TpmsFrameType::Consecutive) {
            break;
        }

        const std::optional<std::array<std::uint8_t, 4>> raw_pressures = can_protocol::getTpmsRawValuesFromFrames(first_frame_buffer, frame);
        if (!raw_pressures) {
            refresh_state = RefreshState::Idle;
            break;
        }
        for (std::size_t i = 0; i < raw_pressures->size(); i++) {
            readings_buffer[static_cast<std::size_t>(config::tire_can_order[i])].raw_pressure = (*raw_pressures)[i];
        }

        if (!can_protocol::sendTpmsTemperaturesRequest()) {
            refresh_state = RefreshState::Idle;
            break;
        }

        refresh_state = RefreshState::AwaitTemperaturesResponse;
        break;
    }

    case RefreshState::AwaitTemperaturesResponse:
        if (*type != can_protocol::TpmsFrameType::Temperatures) {
            break;
        }

        first_frame_buffer = frame;

        if (!can_protocol::sendTpmsFlowControlFrame()) {
            refresh_state = RefreshState::Idle;
            break;
        }

        refresh_state = RefreshState::AwaitTemperaturesConsecutiveResponse;
        break;

    case RefreshState::AwaitTemperaturesConsecutiveResponse: {
        if (*type != can_protocol::TpmsFrameType::Consecutive) {
            break;
        }

        const std::optional<std::array<std::uint8_t, 4>> raw_temperatures = can_protocol::getTpmsRawValuesFromFrames(first_frame_buffer, frame);
        if (!raw_temperatures) {
            refresh_state = RefreshState::Idle;
            break;
        }
        for (std::size_t i = 0; i < raw_temperatures->size(); i++) {
            readings_buffer[static_cast<std::size_t>(config::tire_can_order[i])].raw_temperature = (*raw_temperatures)[i];
        }

        refresh_state = RefreshState::Complete;
        break;
    }

    default:
        break;
    }
}

}
