#pragma once

#include <cstdint>

constexpr uint8_t FrameStart = 0xAA;

enum class MessageId : uint8_t {
    Home      = 'H',
    Stop      = 'S',
    Move      = 'M',
    Telemetry = 'T',
    End       = 'E',
};

struct MovePayload {
    float x;
    float y;
    float z;
};

struct TelemetryPayload {
    float measuredPos[3];
    float measuredVel[3];
    float executedPos;
    float executedVel;
};

struct Command {
    MessageId id;
    MovePayload move;
};
