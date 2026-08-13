#include "robot.h"

#include "motion_controller_utils.h"
#include "robot_types.h"
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
            if(mStateRequest == StateRequest::Home)
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
            if(mStateRequest == StateRequest::Move && mPathSize != 0)
                mMotionController->SetSegments({mPath, mPathSize});
            break;

        case State::Moving: {
            mMotionController->Move();
            mMotionController->Update();
            MotionData steps = mMotionController->GetSteps();
            mStepEngine->SetSteps(steps.x, steps.y, steps.z);
            break;
        }

        case State::Fault:
            mStepEngine->SetSteps(0, 0, 0);
            mHomingTicks = 0;
            mBackoffTicks = 0;
            mPathSize = 0;
            HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
            break;
    }

    // Transiciones de estados
    switch (mState) {
        case State::Init:
            if(mStateRequest == StateRequest::Home)
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
            if(mStateRequest == StateRequest::Home)
                mState = State::Homing;

            if(mStateRequest == StateRequest::Move)
                mState = State::Moving;
            break;

        case State::Moving:
            if(mMotionController->IsFinished()){
                mState = State::Idle;
                mStateRequest = StateRequest::None;
            }
            break;

        case State::Fault:
            if(mStateRequest == StateRequest::Reset)
                mState = State::Init;
            break;
    }
}

void Robot::SetCommandRequest(CommandRequest commandRequest) {
    mStateRequest = commandRequest.stateRequest;
    memcpy(mPath, commandRequest.path, commandRequest.pathSize * sizeof(MotionData));
    mPathSize = commandRequest.pathSize;
}

void Robot::EmergencyStop() {
    mState = State::Fault;
}

CommandRequest Robot::ParseCommand(char* command, uint16_t size) {
    CommandRequest request;
    request.stateRequest = StateRequest::None;
    request.pathSize = 0;

    char* character = command;

    switch (*character) {
        case 'H':
            request.stateRequest = StateRequest::Home;
            break;
        case 'R':
            request.stateRequest = StateRequest::Reset;
            break;
        case 'M':
            request.stateRequest = StateRequest::Move;
            break;
        default:
            break;
    }

    if(request.stateRequest == StateRequest::Move) {
        character++;

        uint16_t segmentCount = 0;

        while(*character != '\0' && segmentCount < MAX_SEGMENTS) {
            if(*character == ' ') {
                character++;
                continue;
            }

            float values[3] {};
            bool valid = true;
            uint8_t coordCount = 0;

            for(auto& value : values) {
                if(*character == '\0')
                    break;

                while(*character == ' ')
                    character++;

                if(*character < '0' || *character > '9') {
                    valid = false;
                    break;
                }

                while(*character >= '0' && *character <= '9') {
                    value = value * 10.0f + (*character - '0');
                    character++;
                }

                coordCount++;
            }

            if(coordCount != 3)
                valid = false;

            if(!valid)
                break;

            request.path[segmentCount].x = values[0];
            request.path[segmentCount].y = values[1];
            request.path[segmentCount].z = values[2];
            segmentCount++;

        }

        request.pathSize = segmentCount;
    }

    return request;
}

std::shared_ptr<IRobot> MakeIRobot(std::shared_ptr<IStepEngine> stepEngine,
        std::shared_ptr<IGpioInput> endStopX,
        std::shared_ptr<IGpioInput> endStopY,
        std::shared_ptr<IGpioInput> endStopZ,
        std::shared_ptr<IComms> comms) {
    return std::make_shared<Robot>(stepEngine, endStopX, endStopY, endStopZ, comms);
}
