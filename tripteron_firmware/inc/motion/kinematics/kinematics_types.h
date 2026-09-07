#pragma once

#include "motion_types.h"

struct KinematicState {
    MotionData pos;
    MotionData vel;
};

enum class KinematicsResult {
    None = 0,
    OK,
    NoData,
    NoSolution
};
