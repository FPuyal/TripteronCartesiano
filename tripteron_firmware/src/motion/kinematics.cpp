#include "kinematics.h"
#include <cmath>

#define DIST    180.0f // Length of the robot arm in mm
#define GAMMAX  76.9044f  // Offset of alphaX in º
#define GAMMAY  77.7114f  // Offset of alphaY in º
#define GAMMAZ  22.5280f  // Offset of alphaZ in º
#define LY      261.6f  // Distance from Y axis to X arm base in mm
#define LX      263.6f  // Distance from X axis to Y arm base in mm
#define DX      85.7f // Distance from X axis to Z arm base in mm
#define DY      35.5f // Distance from Y axis to Z arm base in mm
#define L1      40.0f  // Y Offset of X arm in mm
#define L2      40.0f  // X Offset of Y arm in mm
#define L3      25.0f  // X Offset of Z arm in mm

#define MAX_ITERATIONS 8
#define FK_TOL 0.01f
#define TIME_STEP 0.001f // Time step in seconds

void Kinematics::CaptureHome() {
    mXEncoder->SetOffset();
    mYEncoder->SetOffset();
    mZEncoder->SetOffset();
    HAL_Delay(2);

    CalculateKinematics();
    mHome[0] = mPreviousState.pos[0];
    mHome[1] = mPreviousState.pos[1];
    mHome[2] = mPreviousState.pos[2];
}

void Kinematics::Update() {
    if(mUpdateKinematics) {
        mUpdateKinematics = false;
        CalculateKinematics();
    }
}

void Kinematics::CalculateKinematics() {

    float alphaX, alphaY, alphaZ;

    mXEncoder->ReadAngle(alphaX);
    mYEncoder->ReadAngle(alphaY);
    mZEncoder->ReadAngle(alphaZ);

    auto lawOfCosines = [&](float a_deg){
        const float a = a_deg * 0.01745329252f; // deg→rad
        return 2*DIST*DIST*(1 - cosf(a));
    };

    const float A = lawOfCosines(alphaX + GAMMAX);
    const float B = lawOfCosines(alphaY + GAMMAY);
    const float C = lawOfCosines(alphaZ + GAMMAZ);
    const float P = LY - L1;
    const float Q = LX - L2;
    const float R = DX - L3;
    const float S = DY;

    // Reduccion a una ecuacion escalar en w = z^2:
    //   x(w) = Q - sqrt(B - w)      dx/dw = 1 / (2*sqrt(B - w))
    //   y(w) = P - sqrt(A - w)      dy/dw = 1 / (2*sqrt(A - w))
    //   g(w) = (R + x)^2 + (S + y)^2 - C
    //   g'(w) = (R + x)/sqrt(B - w) + (S + y)/sqrt(A - w)
    // Cotas del modelo: z^2 <= A y z^2 <= B  =>  w en [0, min(A, B)]
    float x = 0.0f, y = 0.0f;
    auto evalG = [&](float w, float& g, float& dg) {
        const float ra = sqrtf(fmaxf(A - w, 0.0f));
        const float rb = sqrtf(fmaxf(B - w, 0.0f));
        x = Q - rb;
        y = P - ra;
        const float Rx = R + x;
        const float Sy = S + y;
        g = Rx*Rx + Sy*Sy - C;
        dg = 0.0f;
        if (rb > 1e-6f) dg += Rx / rb;
        if (ra > 1e-6f) dg += Sy / ra;
    };

    float lo = 0.0f;
    float hi = fminf(A, B);
    float gLo, gHi, dg;
    evalG(lo, gLo, dg);
    evalG(hi, gHi, dg);
    if (gLo * gHi > 0.0f)
        return; // sin raiz en el bracket -> lecturas de encoder inconsistentes

    // Warm start: w del ciclo anterior, saturado al bracket
    float w = mPreviousState.pos[2] * mPreviousState.pos[2];
    if (w < lo) w = lo;
    if (w > hi) w = hi;

    bool converged = false;
    for(int i = 0; i < MAX_ITERATIONS; i++) {
        float g;
        evalG(w, g, dg);
        if (fabsf(g) < FK_TOL) {
            converged = true;
            break;
        }

        // Mantener el bracket
        if (gLo * g < 0.0f) {
            hi = w;
        } else {
            lo = w;
            gLo = g;
        }

        float wn;
        if (fabsf(dg) > 1e-9f) {
            wn = w - g / dg;
            if (!(wn > lo && wn < hi))  // Newton se sale -> biseccion
                wn = 0.5f * (lo + hi);
        } else {
            wn = 0.5f * (lo + hi);
        }
        w = wn;
    }

    if(!converged)
        return;

    const float z = sqrtf(fmaxf(w, 0.0f));

    // salida restada (cosmético)
    mCurrentState.pos[0] = x - mHome[0];
    mCurrentState.pos[1] = y - mHome[1];
    mCurrentState.pos[2] = z - mHome[2];
    mCurrentState.vel[0] = (x - mPreviousState.pos[0]) / TIME_STEP;
    mCurrentState.vel[1] = (y - mPreviousState.pos[1]) / TIME_STEP;
    mCurrentState.vel[2] = (z - mPreviousState.pos[2]) / TIME_STEP;

    // semilla Newton: pose CRUDA
    mPreviousState.pos[0] = x;
    mPreviousState.pos[1] = y;
    mPreviousState.pos[2] = z;
}

std::shared_ptr<IKinematics> MakeIKinematics(std::shared_ptr<IEncoder> xEncoder, std::shared_ptr<IEncoder> yEncoder, std::shared_ptr<IEncoder> zEncoder){
    return std::make_shared<Kinematics>(xEncoder, yEncoder, zEncoder);
}
