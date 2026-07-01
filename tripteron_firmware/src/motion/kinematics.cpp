#include "kinematics.h"
#include <cmath>

#define DIST    100.0f // Length of the robot arm in mm
#define GAMMAX  0.0f   // Offset for X axis in mm
#define GAMMAY  0.0f   // Offset for Y axis in mm
#define GAMMAZ  0.0f   // Offset for Z axis in mm
#define LY      50.0f  // Distance from Y axis to X arm base in mm
#define LX      50.0f  // Distance from X axis to Y arm base in mm
#define DX      -50.0f // Distance from X axis to Z arm base in mm
#define DY      -50.0f // Distance from Y axis to Z arm base in mm
#define L1      10.0f  // Offset from Y axis to end effector in mm
#define L2      10.0f  // Offset from X axis to end effector in mm
#define L3      10.0f  // Offset from X axis to end effector in mm

#define MAX_ITERATIONS 8
#define FK_TOL 0.01f
#define TIME_STEP 0.001f // Time step in seconds

CinematicState Kinematics::Update() {
    if(mUpdateKinematics) {
        mUpdateKinematics = false;
        CalculateKinematics();
    }

    return mCurrentState;
}

void Kinematics::CalculateKinematics() {

    float alphaX, alphaY, alphaZ;

    mXEncoder->ReadAngle(alphaX);
    mYEncoder->ReadAngle(alphaY);
    mZEncoder->ReadAngle(alphaZ);

    auto lawOfCosines = [&](float a){
        return 2*DIST*DIST*(1 - cosf(a));
    };

    const float A = lawOfCosines(alphaX + GAMMAX);
    const float B = lawOfCosines(alphaY + GAMMAY);
    const float C = lawOfCosines(alphaZ + GAMMAZ);
    const float P = LY - L1;
    const float Q = LX - L2;
    const float R = DX - L3;
    const float S = DY;

    float x = mPreviousState.pos[0];
    float y = mPreviousState.pos[1];

    bool converged = false;
    for(int i = 0; i < MAX_ITERATIONS; i++) {
        const float Py = P - y;
        const float Qx = Q - x;
        const float Rx = R + x;
        const float Sy = S + y;

        const float F1 = Py*Py - Qx*Qx - A + B;
        const float F2 = Rx*Rx + Sy*Sy - C;
        if (fabsf(F1) + fabsf(F2) < FK_TOL) {
            converged = true;
            break;
        }

        const float J00 = 2.0f * Qx;
        const float J01 = -2.0f * Py;
        const float J10 = 2.0f * Rx;
        const float J11 = 2.0f * Sy;

        const float detJ = J00 * J11 - J01 * J10;
        if (fabsf(detJ) < 1e-6f)
            break;

        x -= (J11 * F1 - J01 * F2) / detJ;
        y -= (-J10 * F1 + J00 * F2) / detJ;
    }

    if(!converged)
        return;

    const float Py = P - y;
    float zz = A - Py*Py;
    if(zz < 0.0f)
        zz = 0.0f;
    const float z = sqrtf(zz);

    mCurrentState.pos[0] = x;
    mCurrentState.pos[1] = y;
    mCurrentState.pos[2] = z;
    mCurrentState.vel[0] = (mCurrentState.pos[0] - mPreviousState.pos[0]) / TIME_STEP;
    mCurrentState.vel[1] = (mCurrentState.pos[1] - mPreviousState.pos[1]) / TIME_STEP;
    mCurrentState.vel[2] = (mCurrentState.pos[2] - mPreviousState.pos[2]) / TIME_STEP;

    mPreviousState = mCurrentState;

}
