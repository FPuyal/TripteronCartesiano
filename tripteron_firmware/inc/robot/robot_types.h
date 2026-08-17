#pragma once

#include "motion_controller_types.h"

static constexpr uint16_t maxSegments = 64;

enum class State {
    Init,
    Homing,
    Backoff,
    Idle,
    Moving,
    Fault
};

enum class StateRequest {
    None,
    Home,
    Move,
    Reset
};

struct CommandRequest {
    StateRequest stateRequest;
    MotionData path[maxSegments];
    uint16_t pathSize;
};
