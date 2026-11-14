#include "trajectory_generator.h"
#include "trajectory_generator_types.h"

#include <algorithm>
#include <cmath>
#include <memory>

static constexpr float kUpdateDt = 0.001f; // Se define en base a la frecuancia de interrupción del TIM2
static constexpr float eps = 1e-3f;

bool TrajectoryGenerator::SetTrajectoryProfile(MotionState init, MotionState final, TrajectoryConfig config) {
    if(init.pos < 0.0f || final.pos < 0.0f || init.vel < 0.0f || final.vel < 0.0f || final.pos < init.pos)
        return false;

    mConfig = config;

    if(init.vel > mConfig.velMax)
        init.vel = mConfig.velMax;

    if(final.vel > mConfig.velMax)
        final.vel = mConfig.velMax;

    mInit = init;
    mFinal = final;

    mCurrentPhase = 0;
    mPhaseTime = 0.0f;
    mPos0 = mInit.pos;
    mVel0 = mInit.vel;
    mPos = mInit.pos;
    mVel = mInit.vel;
    mAcc = 0.0f;

    if(!GeneratePhases()) {
        mFinished = true;   // nada ejecutable: no dejamos el generador en estado zombi
        return false;
    }

    mFinished = false;
    return true;
}

bool TrajectoryGenerator::GeneratePhases() {
    mNumPhases = 0;

    const float a    = mConfig.accMax;
    const float vmax = mConfig.velMax;
    const float vi   = mInit.vel;              // velocidad de entrada
    const float vf   = mFinal.vel;             // velocidad de salida
    const float d    = mFinal.pos - mInit.pos; // distancia del segmento

    if (a <= 0.0f || d <= eps)
        return false;

    // Pico de un triangular que llena exactamente d.
    const float vp = sqrtf(a * d + 0.5f * (vi * vi + vf * vf));

    // Si el pico no alcanza el mayor de los dos extremos, d es demasiado corta
    // ni para la transición vi->vf respetando la aceleración (lo evita el look-ahead).
    if (vp < std::max(vi, vf) - eps)
        return false;

    // Crucero: triangular (vp) si no llega a vmax; trapezoidal (vmax) si lo supera.
    const float vc = (vp < vmax) ? vp : vmax;

    const float dAcc    = (vc * vc - vi * vi) / (2.0f * a); // >= 0 (vc >= vi)
    const float dDec    = (vc * vc - vf * vf) / (2.0f * a); // >= 0 (vc >= vf)
    const float dCruise = d - dAcc - dDec;

    if (vc > vi + eps)                                                  // aceleración vi -> vc
        mPhases[mNumPhases++] = { +a,   vc, 0.0f, EndCondition::VEL };

    if (dCruise > eps)                                                  // crucero a vc
       mPhases[mNumPhases++] = { 0.0f, vc, dAcc + dCruise, EndCondition::DIST };

    if (vc > vf + eps)
        mPhases[mNumPhases++] = {-a, vf, 0.0f, EndCondition::VEL}; // desaceleración vc -> vf

    return mNumPhases != 0;
}

bool TrajectoryGenerator::Update() {
    if(mNumPhases == 0 || mFinished)
        return false;

    mPhaseTime += kUpdateDt;
    mAcc = mPhases[mCurrentPhase].accLim;
    mVel = mVel0 + mAcc * mPhaseTime;
    mPos = mPos0 + mVel0 * mPhaseTime + 0.5f * mAcc * mPhaseTime * mPhaseTime;

    auto advancePhase = [this]()->void {
        mCurrentPhase++;
        if(mCurrentPhase >= mNumPhases) {
            mFinished = true;
            mPos = mFinal.pos;
            mVel = mFinal.vel;
            mAcc = 0.0f;
            return;
        }
        mPhaseTime = 0.0f;
        mPos0 = mPos;
        mVel0 = mVel;
    };

    if(mPhases[mCurrentPhase].endCondition == EndCondition::VEL){
        float sign = (mPhases[mCurrentPhase].velLim >= mVel0) ? 1.0f : -1.0f;
        if(sign * (mPhases[mCurrentPhase].velLim - mVel) <= eps) {
            mVel = mPhases[mCurrentPhase].velLim;
            mAcc = 0.0f;
            advancePhase();
            return true;
        }
    }

    if(mPhases[mCurrentPhase].endCondition == EndCondition::DIST){
        if((mPhases[mCurrentPhase].posLim - mPos) <= eps) {
            mPos = mPhases[mCurrentPhase].posLim;
            mVel = mPhases[mCurrentPhase].velLim;
            mAcc = 0.0f;
            advancePhase();
            return true;
        }
    }
    return true;
}

void TrajectoryGenerator::Reset() {
    mCurrentPhase = 0;
    mFinished = true;

    mPhaseTime = 0.0f;
    mPos0 = 0.0f;
    mVel0 = 0.0f;

    mPos = 0.0f;
    mVel = 0.0f;
    mAcc = 0.0f;
}

std::unique_ptr<ITrajectoryGenerator> MakeITrajectoryGenerator() {
    return std::make_unique<TrajectoryGenerator>();
}
