#pragma once

struct MotionData {
    float x;
    float y;
    float z;
};

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
};

