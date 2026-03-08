#include "trayectory_profile.h"
#include <cmath>
#include <memory>

void TrajectoryProfile::SetTrajectorySegment(TrajectoryState init, TrajectoryState final) {
    mDir = (final.pos - init.pos >= 0) ? 1.0 : -1.0;

    mTime = 0.0;

    mPos = init.pos;
    mVel = init.vel;
    mAcc = 0.0f;

    mFinished = false;

    double constJerkTime = 0.0;
    double constAccelTime = 0.0;
    double velDir = (final.vel > init.vel) ? 1.0 : -1.0;

    constJerkTime = mAccMax / mJerk;

    double vel_t1 = init.vel + velDir * 0.5 * mJerk * constJerkTime * constJerkTime;
    double vel_t2 = final.vel - velDir * mAccMax * constJerkTime + velDir * 0.5 * mJerk * constJerkTime * constJerkTime;
    constAccelTime = velDir * (vel_t2 - vel_t1) / mAccMax;

    t1 = constJerkTime;
    t2 = t1 + constAccelTime;
    mTotalTime = t2 + constJerkTime;
}

bool TrajectoryProfile::Update(double dt) {
    if (mFinished)
        return false;

    mTime += dt;

    double jerk = mTime < t1 ? mJerk : (mTime < t2 ? 0.0 : -mJerk);

    mAcc += jerk * dt;
    mVel += mAcc * dt;
    mPos += mDir * mVel * dt;

    if(mTime >= mTotalTime)
        mFinished = true;

    return true;
}

std::shared_ptr<ITrajectoryProfile> MakeITrajectoryProfile(TrajectoryConfig config) {
    return std::make_shared<TrajectoryProfile>(config);
}
