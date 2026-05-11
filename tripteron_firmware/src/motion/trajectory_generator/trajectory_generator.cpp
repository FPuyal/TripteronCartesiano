#include "trajectory_generator.h"
#include "trajectory_generator_utils.h"

#include <cmath>
#include <cstdlib>
#include <memory>
#include <limits>
#include <vector>

bool TrajectoryGenerator::SetTrajectoryProfile(MotionState init, MotionState final) {
    if(init.pos < 0.0 || final.pos < 0.0 || init.vel < 0.0 || final.vel < 0.0)
        return false;

    if(init.vel > mConfig.velMax)
        init.vel = mConfig.velMax;

    if(final.vel > mConfig.velMax)
        final.vel = mConfig.velMax;

    mInit = init;
    mFinal = final;

    mCurrentPhase = 0;
    mFinished = false;

    mPhaseTime = 0.0;
    mPos0 = mInit.pos;
    mVel0 = mInit.vel;

    mDir = (mFinal.pos >= mInit.pos) ? 1.0 : -1.0;
    mPos = mInit.pos;
    mVel = mInit.vel;
    mAcc = 0.0;
    // mJerk = 0.0;

    // mProfile = TrayectoryProfileType::NONE;
    mPhases.clear();

    const double dist = std::abs(mFinal.pos - mInit.pos);

    auto assignProfile = [this](TrayectoryProfileType profileType) -> bool {
        mProfile = profileType;
        if (mProfile == TrayectoryProfileType::NONE)
            return false;

        return GeneratePhases();
    };

    auto fits = [this, dist](double initVel, double finalVel) -> bool {
        const double d = CalculateRampDistance(initVel, finalVel, mConfig);
        return d > eps && d <= dist + eps;
    };

    auto fitsCombined = [this, dist](double initVel, double maxVel, double finalVel) -> bool {
        const double d1 = CalculateRampDistance(initVel, maxVel, mConfig);
        const double d2 = CalculateRampDistance(maxVel, finalVel, mConfig);
        return d1 > eps && d2 > eps && (d1 + d2) <= dist + eps;
    };

    if (mFinal.vel >= mConfig.velMax - eps) {
        mFinal.vel = mConfig.velMax;

        if (fits(mInit.vel, mConfig.velMax))
            return assignProfile(TrayectoryProfileType::TRAPEZOIDAL_PARCIAL);

        return false;
    }

    if (fitsCombined(mInit.vel, mConfig.velMax, mFinal.vel))
        return assignProfile(TrayectoryProfileType::TRAPEZOIDAL);

    if (fits(mInit.vel, mFinal.vel))
        return assignProfile(TrayectoryProfileType::TRAPEZOIDAL_PARCIAL);

    double max_vel_aux = sqrt(mConfig.accMax * dist + (mInit.vel * mInit.vel + mFinal.vel * mFinal.vel) / 2);
    if(fitsCombined(mInit.vel, max_vel_aux, mFinal.vel))
        return assignProfile(TrayectoryProfileType::TRIANGULAR);

    return false;
}

bool TrajectoryGenerator::GeneratePhases() {
    mPhases.clear();

    auto addRampProfile = [this](double initVel, double finalVel) -> bool {
        if (initVel < 0.0 || finalVel < 0.0)
            return false;

        double accleSign = (finalVel >= initVel) ? 1.0 : -1.0;

        mPhases.push_back(TrayectoryPhase {
            accleSign * mConfig.accMax,
            finalVel,
            0.0,
            EndCondition::VEL,
        });

        return true;
    };

    auto addCruisePhase = [this](double cruiseVel, TrayectoryProfileType profileType) -> bool {
        if (cruiseVel <= eps || profileType == TrayectoryProfileType::TRIANGULAR)
            return false;

        double pos_aux = mFinal.pos;
        double dist_aux = abs(mFinal.pos - mInit.pos);

        if(profileType == TrayectoryProfileType::TRAPEZOIDAL) {
            double decelDist = CalculateRampDistance(cruiseVel, mFinal.vel, mConfig);
            dist_aux -= CalculateRampDistance(mInit.vel, cruiseVel, mConfig);
            dist_aux -= decelDist;
            pos_aux  -= decelDist;
        }
        if(profileType == TrayectoryProfileType::TRAPEZOIDAL_PARCIAL) {
            dist_aux -= mInit.vel <= mFinal.vel ?
                CalculateRampDistance(mInit.vel, cruiseVel, mConfig) :
                CalculateRampDistance(cruiseVel, mFinal.vel, mConfig);
            pos_aux = mInit.vel <= mFinal.vel ? pos_aux :
                pos_aux - CalculateRampDistance(cruiseVel, mFinal.vel, mConfig);
        }

        if (dist_aux < -eps)
            return false;  // El perfil no cabe
        if (dist_aux <  eps)
            return true;   // Cabe pero sin fase de crucero

        // Necesita fase de crucero
        mPhases.push_back({
            0.0,
            cruiseVel,
            pos_aux,
            EndCondition::DIST
        });

        return true;
    };

    switch (mProfile) {
        case TrayectoryProfileType::TRAPEZOIDAL:
            return addRampProfile(mInit.vel, mConfig.velMax)
            && addCruisePhase(mConfig.velMax, TrayectoryProfileType::TRAPEZOIDAL)
            && addRampProfile(mConfig.velMax, mFinal.vel);
        case TrayectoryProfileType::TRAPEZOIDAL_PARCIAL:
            if(mFinal.vel < mInit.vel - eps)
                return addCruisePhase(mInit.vel, TrayectoryProfileType::TRAPEZOIDAL_PARCIAL) &&
                    addRampProfile(mInit.vel, mFinal.vel);
            return addRampProfile(mInit.vel, mFinal.vel) &&
                addCruisePhase(mFinal.vel, TrayectoryProfileType::TRAPEZOIDAL_PARCIAL);
        case TrayectoryProfileType::TRIANGULAR : {
            double dist = abs(mFinal.pos - mInit.pos);
            double max_vel_aux = sqrt(mConfig.accMax * dist + (mInit.vel * mInit.vel + mFinal.vel * mFinal.vel) / 2);
            return addRampProfile(mInit.vel, max_vel_aux) &&
                addRampProfile(max_vel_aux, mFinal.vel);
        }
        default:
            return false;
    }
}

bool TrajectoryGenerator::Update(double dt) {
    if(dt <= 0.0 || mPhases.empty() || mFinished)
        return false;

    mPhaseTime += dt;
    mAcc = mPhases[mCurrentPhase].accLim;
    mVel = mVel0 + mAcc * mPhaseTime;
    mPos = mPos0 + mVel0 * mPhaseTime + 0.5 * mAcc * mPhaseTime * mPhaseTime;

    auto advancePhase = [this]()->void {
        mCurrentPhase++;
        if(mCurrentPhase >= mPhases.size()) {
            mFinished = true;
            mPos = mFinal.pos;
            mVel = mFinal.vel;
            mAcc = 0.0;
            return;
        }
        mPhaseTime = 0.0;
        mPos0 = mPos;
        mVel0 = mVel;
    };

    if(mPhases[mCurrentPhase].endCondition == EndCondition::VEL){
        double sign = (mPhases[mCurrentPhase].velLim >= mVel0) ? 1.0 : -1.0;
        if(sign * (mPhases[mCurrentPhase].velLim - mVel) <= eps) {
            mVel = mPhases[mCurrentPhase].velLim;
            mAcc = 0.0;
            advancePhase();
            return true;
        }
    }

    if(mPhases[mCurrentPhase].endCondition == EndCondition::DIST){
        if(mDir * (mPhases[mCurrentPhase].posLim - mPos) <= eps) {
            mPos = mPhases[mCurrentPhase].posLim;
            mVel = mPhases[mCurrentPhase].velLim;
            mAcc = 0.0;
            advancePhase();
            return true;
        }
    }

    return true;
}

void TrajectoryGenerator::Reset() {
    mCurrentPhase = 0;
    mFinished = true;

    mPhaseTime = 0.0;
    mPos0 = 0.0;
    mVel0 = 0.0;

    mDir = 0.0;
    mPos = 0.0;
    mVel = 0.0;
    mAcc = 0.0;
}

std::shared_ptr<ITrajectoryGenerator> MakeITrajectoryGenerator(TrajectoryConfig config) {
    return std::make_shared<TrajectoryGenerator>(config);
}
