#pragma once

#include "motion_controller_types.h"

#include <cstdint>

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
    MotionData path[kMaxSegments];
    uint16_t pathSize;
};
