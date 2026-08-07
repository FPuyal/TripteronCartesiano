#pragma once

struct MotionData {
    float x;
    float y;
    float z;
};

struct MotionConfig {
    MotionData pos;
    MotionData vel;
    MotionData acc;
    MotionData stepsPerMm;
};

