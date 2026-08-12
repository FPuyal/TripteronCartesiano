#pragma once

#include "motion_controller_utils.h"

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
    StateRequest state;
    MotionPath path;
};
