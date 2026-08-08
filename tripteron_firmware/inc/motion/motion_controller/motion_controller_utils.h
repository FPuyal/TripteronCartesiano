#pragma once

struct MotionData {
    float x;
    float y;
    float z;
};

struct MotionConfig {
    MotionData posMax;
    MotionData velMax;
    MotionData velMin;
    MotionData accMax;
    MotionData stepsPerMm;
};

