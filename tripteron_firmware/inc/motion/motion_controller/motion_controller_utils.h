#pragma once

struct MotionData {
    float x;
    float y;
    float z;
};

inline MotionData operator*(const MotionData& a, const MotionData& b) {
    MotionData result = a;
    result.x *= b.x;
    result.y *= b.y;
    result.z *= b.z;
    return result;
}

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

