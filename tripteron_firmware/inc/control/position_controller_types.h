#pragma once

#include "motion_types.h"

struct PositionControlConfig {
    MotionData kp;            // ganancia proporcional por eje (1/s)
    MotionData maxCorrection; // corrección máxima de velocidad por eje (mm/s)
    MotionData deadband;      // error por debajo del cual no se corrige (mm)
};
