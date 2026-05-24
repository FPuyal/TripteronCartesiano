#include "axis.h"
#include "axis_types.h"
#include "trajectory_generator_interface.h"
#include "trajectory_generator_utils.h"
#include "utils.h"
#include <sys/types.h>

Axis::Axis(std::shared_ptr<IGpioInput> gpioInput, std::shared_ptr<ITmc> tmc, const AxisConfig& config) :
    mEndStop(gpioInput), mTmc(tmc), mStepsMmRatio(config.stepsMmRatio), mMaxPosition(config.maxPosition) {
    mTrajectoryGenerator = MakeITrajectoryGenerator({config.maxVelocity * mStepsMmRatio, config.maxAcceleration * mStepsMmRatio});
}

void Axis::Tick() {
    // Lógica de cada estado
    switch (mState) {
        case AxisState::Enabling:
            mTmc->Enable();
            break;

        case AxisState::Idle:
            mTmc->SetSpeed(0);
            break;

        case AxisState::Disabling:
            break;

        case AxisState::Disabled:
            break;

        case AxisState::Homing:
            if(!mTmc->SetSpeed(500))
                mState = AxisState::Failing;
            mTmc->SetDirection(false); // Configurar dirección de homing
            if(mEndStop->Read()) {
                mTmc->SetSpeed(0);
            }
            break;

        case AxisState::Backoff:
            if(!mTmc->SetSpeed(200))
                mState = AxisState::Failing;
            mTmc->SetDirection(true); // Configurar dirección de backoff
            if(!mEndStop->Read()) {
                mTmc->SetSpeed(0);
                mCurrentMotionState = {0.0, 0.0};
                mState = AxisState::Idle;
            }
            break;

        case AxisState::Moving:
            if(!mSegment.empty()){
                if(mTrajectoryGenerator->IsFinished() && mCurrentSegment < mSegment.size()) {
                    auto targetState = mSegment[mCurrentSegment] * mStepsMmRatio;
                    if(!mTrajectoryGenerator->SetTrajectoryProfile(mCurrentMotionState * mStepsMmRatio, mSegment[mCurrentSegment] * mStepsMmRatio)) {
                        mState = AxisState::Failing;
                        break;
                    }
                    mTmc->SetDirection(mTrajectoryGenerator->GetDirection());
                    mCurrentSegment++;
                }
                mTrajectoryGenerator->Update(0.001); // Actualizar cada 1 ms. Frecuencia de llamada de tick()
                mCurrentMotionState.pos = mTrajectoryGenerator->GetPosition() / mStepsMmRatio;
                mCurrentMotionState.vel = mTrajectoryGenerator->GetVelocity() / mStepsMmRatio;
                mTmc->SetSpeed(static_cast<uint32_t>(mTrajectoryGenerator->GetVelocity()));
            }
            break;

        case AxisState::Failing:
            mTmc->Disable();
            break;

        case AxisState::Failed:
            break;

        default:
            break;
    }

    // Transiciones de los estados
    switch (mState) {
        case AxisState::Enabling:
            mState = AxisState::Idle;
            break;

        case AxisState::Idle:
            if(mCommandRequest == AxisCommandRequest::Disable)
                mState = AxisState::Disabling;
            else if(mCommandRequest == AxisCommandRequest::Move)
                mState = AxisState::Moving;
            else if(mCommandRequest == AxisCommandRequest::Home)
                mState = AxisState::Homing;
            mCommandRequest = AxisCommandRequest::None;
            break;

        case AxisState::Disabling:
            if(!mTmc->Disable())
                mState = AxisState::Failing;
            else
                mState = AxisState::Disabled;
            break;

        case AxisState::Disabled:
            if(mCommandRequest == AxisCommandRequest::Enable)
                mState = AxisState::Enabling;
            mCommandRequest = AxisCommandRequest::None;
            break;

        case AxisState::Homing:
            mHomingTicks++;
            if(mEndStop->Read()) {
                mHomingTicks = 0;
                mState = AxisState::Backoff;
            }
            else if (mHomingTicks > 10000) { // Timeout de homing (10 segundos a 1 ms por tick)
                mHomingTicks = 0;
                mState = AxisState::Failing;
            }
            break;

        case AxisState::Moving:
            if(mSegment.empty() || mCurrentMotionState.pos > mMaxPosition || mEndStop->Read())
                mState = AxisState::Failing;
            if(mTrajectoryGenerator->IsFinished() && mCurrentSegment >= mSegment.size())
                mState = AxisState::Idle;
            break;

        case AxisState::Failing:
            mState = AxisState::Failed;
            break;

        case AxisState::Failed:
            break;

        default:
            break;
    }
}

bool Axis::RequestState(AxisCommandRequest command) {
    if (mState != AxisState::Idle && mState != AxisState::Disabled)
        return false;
    mCommandRequest = command;
    return true;
}

bool Axis::SetSegment(const std::vector<MotionState>& segment) {
    if(mState != AxisState::Idle)
        return false;
    mSegment.clear();
    mSegment = segment;
    mCurrentSegment = 0;
    return true;
}

void Axis::EmergencyStop() {
    mState = AxisState::Failing;
}

AxisState Axis::GetState() const {
    return mState;
}

double Axis::GetPosition() const {
    return mCurrentMotionState.pos;
}

double Axis::GetVelocity() const {
    return mCurrentMotionState.vel;
}

std::shared_ptr<IAxis> MakeIAxis(std::shared_ptr<IGpioInput> gpioInput, std::shared_ptr<ITmc> tmc, const AxisConfig& config) {
    return std::make_shared<Axis>(gpioInput, tmc, config);
}




