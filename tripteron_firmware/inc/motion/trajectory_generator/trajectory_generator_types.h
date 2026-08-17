#pragma once

#include <vector>

struct MotionState {
    float pos;
    float vel;
};

inline MotionState operator*(const MotionState& a, float scalar) {
    return MotionState{a.pos * scalar, a.vel * scalar};
}

struct TrajectoryConfig {
    float velMax;
    float accMax;
};

enum class TrayectoryProfileType {
    TRAPEZOIDAL_PARCIAL = 0,
    TRAPEZOIDAL,
    TRIANGULAR,
    NONE
};

enum class EndCondition {
    VEL = 0,
    DIST
};

struct TrayectoryPhase {
    float accLim;
    float velLim;
    float posLim;
    EndCondition endCondition;
};

constexpr float eps = 1e-3;

inline float CalculateRampDistance(float initVel, float finalVel, TrajectoryConfig config) {
    float distance = finalVel >= initVel ?
        (finalVel * finalVel - initVel * initVel) / (2.0 * config.accMax) :
        (initVel * initVel - finalVel * finalVel) / (2.0 * config.accMax);
    return distance;
}
