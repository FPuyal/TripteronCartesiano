#pragma once

#include "motion_types.h"

#include <cstdint>

inline constexpr uint16_t kMaxSegments = 20;

struct MotionConfig {
    MotionData posMax;
    MotionData velMax;
    MotionData accMax;
    float junctionDeviation;
    MotionData stepsPerMm;
};

struct SegmentData {
    MotionData posTarget;
    MotionData cos;
    float dist;
    float velMaxSeg;  // velMax proyectado sobre la dirección del segmento
    float accMaxSeg;  // accMax proyectado sobre la dirección del segmento
    float finalVel;   // velocidad de salida ya acotada (junction + backward + forward)
};

struct MotionPath {
    MotionData* segments;
    uint16_t size;
};

