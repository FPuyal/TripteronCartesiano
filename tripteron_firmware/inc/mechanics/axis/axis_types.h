#pragma once

enum class AxisState {
    Enabling,
    Idle,
    Disabling,
    Disabled,
    Homing,
    Backoff,
    Moving,
    Failing,
    Failed
};

enum class AxisCommandRequest {
    Enable,
    Disable,
    Home,
    Move,
    None
};
