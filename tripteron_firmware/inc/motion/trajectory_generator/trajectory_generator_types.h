#pragma once


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

enum class EndCondition {
    VEL = 0,
    DIST
};

struct TrajectoryPhase {
    float accLim;
    float velLim;
    float posLim;
    EndCondition endCondition;
};
