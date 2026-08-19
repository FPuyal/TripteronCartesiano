#pragma once

struct MotionData {
    float x;
    float y;
    float z;

    MotionData operator*(const MotionData& other) const {
        MotionData result = *this;
        result.x *= other.x;
        result.y *= other.y;
        result.z *= other.z;
        return result;
    }
};
