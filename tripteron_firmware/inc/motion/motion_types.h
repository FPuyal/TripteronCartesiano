#pragma once

struct MotionData {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    MotionData operator*(const MotionData& other) const {
        MotionData result = *this;
        result.x *= other.x;
        result.y *= other.y;
        result.z *= other.z;
        return result;
    }
};
