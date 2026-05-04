#pragma once

#include <vector>

struct MotionState {
    double pos;
    double vel;
};

struct TrajectoryConfig {
    double velMax;
    double accMax;
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
    double accLim;
    double velLim;
    double posLim;
    EndCondition endCondition;
};

constexpr double eps = 1e-3;

inline double CalculateRampDistance(double initVel, double finalVel, TrajectoryConfig config) {
    double distance = finalVel >= initVel ?
        (finalVel * finalVel - initVel * initVel) / (2.0 * config.accMax) :
        (initVel * initVel - finalVel * finalVel) / (2.0 * config.accMax);
    return distance;
}
