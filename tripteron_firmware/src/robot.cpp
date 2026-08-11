#include "robot.h"

#include "stm32f4xx_hal.h"

#include <memory>

#define HOMING_TIMEOUT_TICKS 5000
#define BACKOFF_TIMEOUT_TICKS 1000

Robot::Robot(std::shared_ptr<IStepEngine> stepEngine,
        std::shared_ptr<IGpioInput> endStopX,
        std::shared_ptr<IGpioInput> endStopY,
        std::shared_ptr<IGpioInput> endStopZ,
        std::shared_ptr<IComms> comms) :
        mStepEngine(stepEngine),
        mEndStopX(endStopX),
        mEndStopY(endStopY),
        mEndStopZ(endStopZ),
        mComms(comms) {
    mMotionController = MakeIMotionController(
        MotionConfig{
            {200.0f, 200.0f, 200.0f},
            {240.0f, 240.0f, 48.0f},
            {480.0f, 480.0f, 96.0f},
            0.5f,
            {20.0f, 20.0f, 25.0f}});
}

void Robot::Tick(){
    // Lógica de cada estado
    switch (mState) {
        case State::Init:
            if(mCommandRequest.state == StateRequest::Home)
                HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
            break;

        case State::Homing:
            mStepEngine->SetSteps(
                mEndStopX->Read() ? 0 : -500,
                mEndStopY->Read() ? 0 : -500,
                mEndStopZ->Read() ? 0 : -500);

            if(mHomingTicks++ > HOMING_TIMEOUT_TICKS)
                mStepEngine->SetSteps(0, 0, 0);
            break;

        case State::Backoff:
            mStepEngine->SetSteps(
                mEndStopX->Read() ? 100 : 0,
                mEndStopY->Read() ? 100 : 0,
                mEndStopZ->Read() ? 100 : 0);

            if(mBackoffTicks++ > BACKOFF_TIMEOUT_TICKS)
                mStepEngine->SetSteps(0, 0, 0);

            if(!mEndStopX->Read() && !mEndStopY->Read() && !mEndStopZ->Read()) {
                mMotionController->SetHomePosition();
                HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
            }
            break;

        case State::Idle:
            if(mCommandRequest.state == StateRequest::Move) {
                // Parser que interprete el comando de mCommanRequest.command y gener los segmentos
                // Se pasan los segmentos a mMotionController
            }
            break;

        case State::Moving:
            mMotionController->Move();
            mMotionController->Update();
            // Se settea los pasos de mStepEngine
            break;

        case State::Fault:
            mHomingTicks = 0;
            mBackoffTicks = 0;
            mStepEngine->SetSteps(0, 0, 0);
            // TODO: Fault robot
            break;
    }

    // Transiciones de estados
    switch (mState) {
        case State::Init:
            if(mCommandRequest.state == StateRequest::Home)
                mState = State::Homing;
            break;

        case State::Homing:
            if(mEndStopX->Read() && mEndStopY->Read() && mEndStopZ->Read())
                mState = State::Backoff;

            if(mHomingTicks > HOMING_TIMEOUT_TICKS)
                mState = State::Fault;
            break;

        case State::Backoff:
            if(!mEndStopX->Read() && !mEndStopY->Read() && !mEndStopZ->Read())
                mState = State::Idle;

            if(mBackoffTicks > BACKOFF_TIMEOUT_TICKS)
                mState = State::Fault;
            break;

        case State::Idle:
            if(mCommandRequest.state == StateRequest::Home)
                mState = State::Homing;

            if(mCommandRequest.state == StateRequest::Move)
                mState = State::Moving;
            break;

        case State::Moving:
            if(mMotionController->IsFinished())
                mState = State::Idle;
            break;

        case State::Fault:
            if(mCommandRequest.state == StateRequest::RESET)
                mState = State::Init;
            break;
    }
}

void Robot::EmergencyStop() {
    mStepEngine->SetSteps(0, 0, 0);
    mState = State::Fault;
}

std::shared_ptr<IRobot> MakeIRobot(std::shared_ptr<IStepEngine> stepEngine,
        std::shared_ptr<IGpioInput> endStopX,
        std::shared_ptr<IGpioInput> endStopY,
        std::shared_ptr<IGpioInput> endStopZ,
        std::shared_ptr<IComms> comms) {
    return std::make_shared<Robot>(stepEngine, endStopX, endStopY, endStopZ, comms);
}
