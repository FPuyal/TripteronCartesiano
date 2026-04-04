#include "trajectory_generator.h"

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
    mAcc0 = 0.0;

    mDir = (mFinal.pos >= mInit.pos) ? 1.0 : -1.0;
    mPos = mInit.pos;
    mVel = mInit.vel;
    mAcc = 0.0;
    mJerk = 0.0;

    mProfile = TrayectoryProfileType::NONE;
    mPhases.clear();

    const double dist = std::abs(mFinal.pos - mInit.pos);

    auto assignProfile = [this](TrayectoryProfileType profileType) -> bool {
        mProfile = profileType;
        if (mProfile == TrayectoryProfileType::NONE)
            return false;

        return GeneratePhases();
    };

    auto fits = [this, dist](double initVel, double finalVel, TrayectoryProfileType profileType) -> bool {
        const double d = CalculatePartialProfileDistance(initVel, finalVel, mConfig, profileType);
        return d > eps && d <= dist + eps;
    };

    auto fitsCombined = [this, dist](double initVel, double cruiseVel, double finalVel,
                                          TrayectoryProfileType partialProfile) -> bool {
        const double d1 = CalculatePartialProfileDistance(initVel, cruiseVel, mConfig, partialProfile);
        const double d2 = CalculatePartialProfileDistance(cruiseVel, finalVel, mConfig, partialProfile);
        return d1 > eps && d2 > eps && (d1 + d2) <= dist + eps;
    };

    if (mFinal.vel >= mConfig.velMax - eps) {
        mFinal.vel = mConfig.velMax;
        if (fits(mInit.vel, mConfig.velMax, TrayectoryProfileType::S_CURVE_PARCIAL))
            return assignProfile(TrayectoryProfileType::S_CURVE_PARCIAL);

        if (fits(mInit.vel, mConfig.velMax, TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL))
            return assignProfile(TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL);

        if (fits(mInit.vel, mConfig.velMax, TrayectoryProfileType::TRAPEZOIDAL_PARCIAL))
            return assignProfile(TrayectoryProfileType::TRAPEZOIDAL_PARCIAL);

        return false;
    }

    if (fitsCombined(mInit.vel, mConfig.velMax, mFinal.vel, TrayectoryProfileType::S_CURVE_PARCIAL))
        return assignProfile(TrayectoryProfileType::S_CURVE);

    if (fitsCombined(mInit.vel, mConfig.velMax, mFinal.vel, TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL))
        return assignProfile(TrayectoryProfileType::S_CURVE_TRAPEZOIDAL);

    if (fits(mInit.vel, mFinal.vel, TrayectoryProfileType::S_CURVE_PARCIAL))
        return assignProfile(TrayectoryProfileType::S_CURVE_PARCIAL);

    if (fits(mInit.vel, mFinal.vel, TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL))
        return assignProfile(TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL);

    if (fitsCombined(mInit.vel, mConfig.velMax, mFinal.vel, TrayectoryProfileType::TRAPEZOIDAL_PARCIAL))
        return assignProfile(TrayectoryProfileType::TRAPEZOIDAL);

    if (fits(mInit.vel, mFinal.vel, TrayectoryProfileType::TRAPEZOIDAL_PARCIAL))
        return assignProfile(TrayectoryProfileType::TRAPEZOIDAL_PARCIAL);

    return false;
}

bool TrajectoryGenerator::GeneratePhases() {
    mPhases.clear();
    mCurrentPhase = 0;

    auto addPartialProfile = [this](double initVel, double finalVel, TrayectoryProfileType profileType) -> bool {
        if (initVel < 0.0 || finalVel < 0.0)
            return false;

        double accleSign = (finalVel >= initVel) ? 1.0 : -1.0;
        double aux_vel = 0.0;

        std::vector<TrayectoryPhase> aux_phases;

        aux_phases.push_back(TrayectoryPhase {
            accleSign * mConfig.jerk,
            accleSign * mConfig.accMax,
            0.0,
            0.0,
            EndCondition::ACCEL,
        });

        aux_vel = finalVel - accleSign * (mConfig.accMax * mConfig.accMax) / (2.0 * mConfig.jerk);

        aux_phases.push_back(TrayectoryPhase {
            0.0,
            accleSign * mConfig.accMax,
            aux_vel,
            0.0,
            EndCondition::VEL,
        });

        aux_phases.push_back(TrayectoryPhase {
            -accleSign * mConfig.jerk,
            0.0,
            finalVel,
            0.0,
            EndCondition::ACCEL,
        });

        if(profileType != TrayectoryProfileType::S_CURVE_PARCIAL)
            aux_phases.erase(aux_phases.begin());

        if(profileType == TrayectoryProfileType::TRAPEZOIDAL_PARCIAL) {
            aux_phases.pop_back();
            aux_phases.front().velLim = finalVel;
        }

        mPhases.insert(mPhases.end(), aux_phases.begin(), aux_phases.end());
        return true;
    };

    auto addCruisePhase = [this](double cruiseVel, TrayectoryProfileType profileType) -> bool {
        if (cruiseVel <= eps || profileType == TrayectoryProfileType::NONE)
            return false;

        double aux_pos = 0.0;
        double aux_dist = 0.0;

        if (static_cast<int>(profileType) < 3) {
            aux_dist = std::abs(mFinal.pos - mInit.pos);
            aux_dist -= CalculatePartialProfileDistance(mInit.vel, mFinal.vel, mConfig, profileType);

            if (aux_dist < -eps)
                return false;
            if (std::abs(aux_dist) > eps) {
                mPhases.push_back({
                    0.0,
                    0.0,
                    cruiseVel,
                    mInit.vel < (mFinal.vel - eps) ? mFinal.pos : mInit.pos + aux_dist,
                    EndCondition::DIST
                });
            }

            return true;
        }
        else {
            aux_dist = std::abs(mFinal.pos - mInit.pos);
            aux_pos = CalculatePartialProfileDistance(
                mInit.vel, mConfig.velMax, mConfig,
                static_cast<TrayectoryProfileType>(static_cast<int>(profileType) - 3)
            );
            aux_dist -= aux_pos;
            aux_dist -= CalculatePartialProfileDistance(
                mConfig.velMax, mFinal.vel, mConfig,
                static_cast<TrayectoryProfileType>(static_cast<int>(profileType) - 3)
            );

            if (aux_dist < -eps)
                return false;
            if (std::abs(aux_dist) > eps){
                mPhases.push_back({
                    0.0,
                    0.0,
                    mConfig.velMax,
                    mInit.pos + aux_pos + aux_dist,
                    EndCondition::DIST
                });
            }

            return true;
        }
    };

    auto addPartialProfileWithCruise = [this, &addPartialProfile, &addCruisePhase]
        (TrayectoryProfileType partialProfile) -> bool {
        if (std::abs(mInit.vel - mFinal.vel) < eps)
            return false;

        if (mInit.vel < mFinal.vel - eps) {
            return addPartialProfile(mInit.vel, mFinal.vel, partialProfile) &&
                   addCruisePhase(mFinal.vel, partialProfile);
        }
        else {
            return addCruisePhase(mInit.vel, partialProfile) &&
                   addPartialProfile(mInit.vel, mFinal.vel, partialProfile);
        }
    };

    switch (mProfile) {
        case TrayectoryProfileType::S_CURVE_PARCIAL:
            return addPartialProfileWithCruise(TrayectoryProfileType::S_CURVE_PARCIAL);

        case TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL:
            return addPartialProfileWithCruise(TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL);

        case TrayectoryProfileType::TRAPEZOIDAL_PARCIAL:
            return addPartialProfileWithCruise(TrayectoryProfileType::TRAPEZOIDAL_PARCIAL);

        case TrayectoryProfileType::S_CURVE:
            return addPartialProfile(mInit.vel, mConfig.velMax, TrayectoryProfileType::S_CURVE_PARCIAL) &&
                addCruisePhase(mConfig.velMax, TrayectoryProfileType::S_CURVE) &&
                addPartialProfile(mConfig.velMax, mFinal.vel, TrayectoryProfileType::S_CURVE_PARCIAL);

        case TrayectoryProfileType::S_CURVE_TRAPEZOIDAL:
            return addPartialProfile(mInit.vel, mConfig.velMax, TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL) &&
                addCruisePhase(mConfig.velMax, TrayectoryProfileType::S_CURVE_TRAPEZOIDAL) &&
                addPartialProfile(mConfig.velMax, mFinal.vel, TrayectoryProfileType::S_CURVE_TRAPEZOIDAL_PARCIAL);

        case TrayectoryProfileType::TRAPEZOIDAL:
            return addPartialProfile(mInit.vel, mConfig.velMax, TrayectoryProfileType::TRAPEZOIDAL_PARCIAL) &&
                addCruisePhase(mConfig.velMax, TrayectoryProfileType::TRAPEZOIDAL) &&
                addPartialProfile(mConfig.velMax, mFinal.vel, TrayectoryProfileType::TRAPEZOIDAL_PARCIAL);

        default:
            return false;
    }
}

bool TrajectoryGenerator::Update(double dt) {
    if(dt <= 0.0 || mPhases.empty() || mFinished)
        return false;

    if (mCurrentPhase == 0 && mPhases[mCurrentPhase].jerk == 0.0)
        mAcc0 = mPhases[0].accLim;

    mPhaseTime += dt;
    mJerk = mPhases[mCurrentPhase].jerk;
    mAcc = mAcc0 + mJerk * mPhaseTime;
    mPos = mPos0 + mVel0 * mPhaseTime + 0.5 * mAcc0 * mPhaseTime * mPhaseTime + (1.0/6.0) * mJerk * mPhaseTime * mPhaseTime * mPhaseTime;
    mVel = mVel0 + mAcc0 * mPhaseTime + 0.5 * mJerk * mPhaseTime * mPhaseTime;

    auto advancePhase = [this]()->void {
        mCurrentPhase++;
        if(mCurrentPhase >= mPhases.size()) {
            mFinished = true;
            mPos = mFinal.pos;
            mVel = mFinal.vel;
            mAcc = 0.0;
            mJerk = 0.0;
            return;
        }
        mPhaseTime = 0.0;
        mPos0 = mPos;
        mVel0 = mVel;
        mAcc0 = mPhases[mCurrentPhase].jerk == 0.0 ? mPhases[mCurrentPhase].accLim : mAcc;
    };

    if(mPhases[mCurrentPhase].endCondition == EndCondition::ACCEL){
        double sign = (mPhases[mCurrentPhase].accLim >= mAcc0) ? 1.0 : -1.0;
        if(sign * (mPhases[mCurrentPhase].accLim - mAcc) <= eps) {
            mAcc = mPhases[mCurrentPhase].accLim;
            advancePhase();
            return true;
        }
    }

    if(mPhases[mCurrentPhase].endCondition == EndCondition::VEL){
        double sign = (mPhases[mCurrentPhase].velLim >= mVel0) ? 1.0 : -1.0;
        if(sign * (mPhases[mCurrentPhase].velLim - mVel) <= eps) {
            mVel = mPhases[mCurrentPhase].velLim;
            advancePhase();
            return true;
        }
    }

    if(mPhases[mCurrentPhase].endCondition == EndCondition::DIST){
        if(mDir * (mPhases[mCurrentPhase].posLim - mPos) <= eps) {
            mPos = mPhases[mCurrentPhase].posLim;
            advancePhase();
            return true;
        }
    }

    return true;
}

std::shared_ptr<ITrajectoryGenerator> MakeITrajectoryGenerator(TrajectoryConfig config) {
    return std::make_shared<TrajectoryGenerator>(config);
}
