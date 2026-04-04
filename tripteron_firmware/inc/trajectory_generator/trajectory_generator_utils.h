#pragma once

#include <vector>

struct MotionState {
    double pos;
    double vel;
};

struct TrajectoryConfig {
    double velMax;
    double accMax;
    double jerk;
};

enum class TrayectoryProfileType {
    S_CURVE_PARCIAL = 0,
    S_CURVE_TRAPEZOIDAL_PARCIAL = 1,
    TRAPEZOIDAL_PARCIAL = 2,
    S_CURVE,
    S_CURVE_TRAPEZOIDAL,
    TRAPEZOIDAL,
    NONE
};

enum class EndCondition {
    ACCEL = 0,
    VEL,
    DIST
};

struct TrayectoryPhase {
    double jerk;
    double accLim;
    double velLim;
    double posLim;
    EndCondition endCondition;
};

constexpr double eps = 1e-3;

double CalculatePartialProfileDistance(double initVel, double finalVel, TrajectoryConfig config, TrayectoryProfileType profileType);
