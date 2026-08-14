#pragma once

#include "motion_controller_utils.h"

#define MAX_SEGMENTS 20

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
    MotionData path[MAX_SEGMENTS];
    uint16_t pathSize;
};
