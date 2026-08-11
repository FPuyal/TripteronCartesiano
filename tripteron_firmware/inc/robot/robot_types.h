#pragma once

enum class State {
    Init,
    Homing,
    Backoff,
    Idle,
    Moving,
    Fault
};

enum class StateRequest {
    Home,
    Move,
    RESET
};

struct CommandRequest {
    StateRequest state;
    uint16_t command;
};
