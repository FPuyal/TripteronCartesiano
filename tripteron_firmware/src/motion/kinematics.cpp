#include "kinematics.h"
#include "kinematics_types.h"
#include <cmath>

namespace {
    constexpr float kDist   = 180.0f;    // Longitud del brazo en mm
    constexpr float kGammaX = 76.9044f;  // Offset de alphaX en º
    constexpr float kGammaY = 77.7114f;
    constexpr float kGammaZ = 22.5280f;
    constexpr float kLY     = 261.6f;
    constexpr float kLX     = 263.6f;
    constexpr float kDX     = 85.7f;
    constexpr float kDY     = 35.5f;
    constexpr float kL1     = 40.0f;
    constexpr float kL2     = 40.0f;
    constexpr float kL3     = 25.0f;

    constexpr int   kMaxIterations = 8;
    constexpr float kFkTol         = 0.01f;
    constexpr float kTimeStep      = 0.001f;

    constexpr float kDegToRad = 0.01745329252f;
}

void Kinematics::CaptureHome() {
    if (!mXEncoder->SetOffset()) return;
    if (!mYEncoder->SetOffset()) return;
    if (!mZEncoder->SetOffset()) return;
    HAL_Delay(2);

    CalculateKinematics();
    mHome.x = mPreviousState.pos.x;
    mHome.y = mPreviousState.pos.y;
    mHome.z = mPreviousState.pos.z;
}

void Kinematics::Update() {
    if(mUpdateKinematics) {
        mUpdateKinematics = false;
        CalculateKinematics();
    }
}

void Kinematics::CalculateKinematics() {

    float alphaX = 0.0f, alphaY = 0.0f, alphaZ = 0.0f;
    if (!mXEncoder->ReadAngle(alphaX)) return;
    if (!mYEncoder->ReadAngle(alphaY)) return;
    if (!mZEncoder->ReadAngle(alphaZ)) return;

    auto lawOfCosines = [&](float a_deg){
        const float a = a_deg * kDegToRad; // deg→rad
        return 2*kDist*kDist*(1 - cosf(a));
    };

    const float A = lawOfCosines(alphaX + kGammaX);
    const float B = lawOfCosines(alphaY + kGammaY);
    const float C = lawOfCosines(alphaZ + kGammaZ);
    const float P = kLY - kL1;
    const float Q = kLX - kL2;
    const float R = kDX - kL3;
    const float S = kDY;

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
    float w = mPreviousState.pos.z * mPreviousState.pos.z;
    if (w < lo) w = lo;
    if (w > hi) w = hi;

    bool converged = false;
    for(int i = 0; i < kMaxIterations; i++) {
        float g;
        evalG(w, g, dg);
        if (fabsf(g) < kFkTol) {
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
    mCurrentState.pos.x = x - mHome.x;
    mCurrentState.pos.y = y - mHome.y;
    mCurrentState.pos.z = z - mHome.z;
    mCurrentState.vel.x = (x - mPreviousState.pos.x) / kTimeStep;
    mCurrentState.vel.y = (y - mPreviousState.pos.y) / kTimeStep;
    mCurrentState.vel.z = (z - mPreviousState.pos.z) / kTimeStep;

    // semilla Newton: pose CRUDA
    mPreviousState.pos.x = x;
    mPreviousState.pos.y = y;
    mPreviousState.pos.z = z;
}

std::shared_ptr<IKinematics> MakeIKinematics(std::shared_ptr<IEncoder> xEncoder, std::shared_ptr<IEncoder> yEncoder, std::shared_ptr<IEncoder> zEncoder){
    return std::make_shared<Kinematics>(xEncoder, yEncoder, zEncoder);
}
