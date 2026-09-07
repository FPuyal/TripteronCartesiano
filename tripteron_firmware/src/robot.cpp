#include "robot.h"

#include "comms_interface.h"
#include "kinematics_interface.h"
#include "kinematics_types.h"
#include "motion_controller_interface.h"
#include "motion_controller_types.h"
#include "robot_types.h"
#include "stm32f4xx_hal.h"

#include <memory>
#include <cstdint>
#include <cstring>
#include <cmath>

extern IMotionController* motionControllerInstance;

Robot::Robot(std::shared_ptr<IStepEngine> stepEngine,
        std::shared_ptr<IGpioInput> endStopX,
        std::shared_ptr<IGpioInput> endStopY,
        std::shared_ptr<IGpioInput> endStopZ,
        std::shared_ptr<IKinematics> kinematics,
        std::shared_ptr<IComms> comms) :
        mStepEngine(stepEngine),
        mEndStopX(endStopX),
        mEndStopY(endStopY),
        mEndStopZ(endStopZ),
        mKinematics(kinematics),
        mComms(comms) {
    mMotionController = MakeIMotionController(
        MotionConfig{
            {200.0f, 200.0f, 200.0f},
            {480.0f, 480.0f, 96.0f},
            {960.0f, 960.0f, 192.0f},
            0.2f,
            {20.0f, 20.0f, 25.0f}});
    motionControllerInstance = mMotionController.get();

    mComms->RegisterCommandCallback([this](uint8_t* data, uint16_t len){
        SetCommandRequest(ParseCommand(data, len));
    });
}

void Robot::Run(){
    bool endX = mEndStopX->Read();
    bool endY = mEndStopY->Read();
    bool endZ = mEndStopZ->Read();
    bool segmentFlag = false;
    // Lógica de cada estado
    switch (mState) {
        case State::Init:
            HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
            if(mStateRequest == StateRequest::Home)
                mHomingMs = HAL_GetTick();
            break;

        case State::Homing:
            mStepEngine->SetSteps(
                endX ? 0 : -500,
                endY ? 0 : -500,
                endZ ? 0 : -500);

            mElapsedMs = HAL_GetTick() - mHomingMs;
            if(mElapsedMs > kHomingTimeoutMs)
                mStepEngine->SetSteps(0, 0, 0);

            if(endX && endY && endZ)
                mBackoffMs = HAL_GetTick();
            break;

        case State::Backoff:
            mStepEngine->SetSteps(
                endX ? 100 : 0,
                endY ? 100 : 0,
                endZ ? 100 : 0);

            if(!endX && !endY && !endZ) {
                mMotionController->SetHomePosition();
                if(!mKinematics->CaptureHome()) {
                    mFail = RobotFailure::CaptureHome;
                    break;
                }
            }

            mElapsedMs = HAL_GetTick() - mBackoffMs;
            if(mElapsedMs > kBackoffTimeoutMs)
                mStepEngine->SetSteps(0, 0, 0);

            break;

        case State::Idle:
            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
            HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
            if(mStateRequest == StateRequest::Move)
                segmentFlag = mMotionController->SetSegments({mPath, mPathSize});

            if(mStateRequest == StateRequest::Home) {
                HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
                mHomingMs = HAL_GetTick();
            }
            break;

        case State::Moving: {
            mMotionController->Move();

            if(!mKinematics->Update()) {
                mFail = RobotFailure::KinematicsUpdate;
                break;
            }

            MotionData steps = mMotionController->GetSteps();
            mStepEngine->SetSteps(steps.x, steps.y, steps.z);

            SendTelemetryData();

            if(mMotionController->IsFinished())
                mComms->SendData(&mEndByte, 1);
            break;
        }

        case State::Fault:
            mStepEngine->SetSteps(0, 0, 0);
            mPathSize = 0;
            HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

            if(mStateRequest == StateRequest::Reset)
                mFail = RobotFailure::None;

            break;
    }

    if(mFail != RobotFailure::None)
        mState = State::Fault;

    // Transiciones de estados
    switch (mState) {
        case State::Init:
            if(mStateRequest == StateRequest::Home){
                mState = State::Homing;
                mStateRequest = StateRequest::None;
            }
            break;

        case State::Homing:
            if(endX && endY && endZ)
                mState = State::Backoff;

            else if(mElapsedMs > kHomingTimeoutMs)
                mState = State::Fault;
            break;

        case State::Backoff:
            if(!endX && !endY && !endZ)
                mState = State::Idle;

            else if(mElapsedMs > kBackoffTimeoutMs)
                mState = State::Fault;
            break;

        case State::Idle:
            if(mStateRequest == StateRequest::Home){
                mState = State::Homing;
                mStateRequest = StateRequest::None;
            }

            if(mStateRequest == StateRequest::Move){
                if(segmentFlag)
                    mState = State::Moving;
                else
                    mState = State::Fault;
                mStateRequest = StateRequest::None;
            }
            break;

        case State::Moving:
            if(mMotionController->IsFinished()){
                mState = State::Idle;
                mStateRequest = StateRequest::None;
            }
            break;

        case State::Fault:
            if(mStateRequest == StateRequest::Reset){
                mState = State::Init;
                mStateRequest = StateRequest::None;
            }
            break;
    }
}

void Robot::SetCommandRequest(const CommandRequest& commandRequest) {
    mStateRequest = commandRequest.stateRequest;
    memcpy(mPath, commandRequest.path, commandRequest.pathSize * sizeof(MotionData));
    mPathSize = commandRequest.pathSize;
}

void Robot::RequestTelemetry() {
    mTelemetryFlag = true;
}

bool Robot::SendTelemetryData() {
    if(mTelemetryFlag) {
        mTelemetryFlag = false;

        MotionData pos = mMotionController->GetPosition();
        MotionData vel = mMotionController->GetVelocity();
        float motion[6] = {pos.x, pos.y, pos.z,
            vel.x, vel.y, vel.z};

        MotionData kinematicsPos = {mKinematics->GetCurrentState().pos};
        MotionData kinematicsVel = mKinematics->GetCurrentState().vel;
        float kinematics[6] = {kinematicsPos.x, kinematicsPos.y, kinematicsPos.z,
            kinematicsVel.x, kinematicsVel.y, kinematicsVel.z};

        auto parseData = [](float data, uint8_t* parsedData) {
            int scaleData = (int)lroundf(fabsf(data) * 100.0f);
            parsedData[0] = ' ';
            parsedData[1] = data >= 0 ? '+' : '-';
            parsedData[2] = '0' + (scaleData / 10000) % 10;
            parsedData[3] = '0' + (scaleData / 1000) % 10;
            parsedData[4] = '0' + (scaleData / 100) % 10;
            parsedData[5] = ',';
            parsedData[6] = '0' + (scaleData / 10) % 10;
            parsedData[7] = '0' +  scaleData % 10;
        };

        uint8_t* bufferPtr = mTelemetryBuffer;

        memcpy(bufferPtr, "[MC]", 4);
        bufferPtr += 4;
        for(auto i : motion){
            parseData(i, bufferPtr);
            bufferPtr += 8;
        }

        *(bufferPtr++) = ' ';

        memcpy(bufferPtr, "[KM]", 4);
        bufferPtr += 4;
        for(auto i : kinematics){
            parseData(i, bufferPtr);
            bufferPtr += 8;
        }

        return mComms->SendData(mTelemetryBuffer, sizeof(mTelemetryBuffer));
    }

    return false;
}

void Robot::EmergencyStop() {
    mState = State::Fault;
}

CommandRequest Robot::ParseCommand(uint8_t* command, uint16_t size) {
    CommandRequest request;
    request.stateRequest = StateRequest::None;
    request.pathSize = 0;

    uint16_t commandIdx = 0;

    uint8_t* character = command;

    auto getCharacter = [&](uint16_t idx) -> uint8_t {
        return idx < size ? character[idx] : '\0';
    };

    switch (getCharacter(commandIdx)) {
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
        commandIdx++;

        uint16_t segmentCount = 0;

        while(getCharacter(commandIdx) != '\0' && segmentCount < kMaxSegments) {
            if(getCharacter(commandIdx) == ' ') {
                commandIdx++;
                continue;
            }

            float values[3] {};
            bool valid = true;
            uint8_t coordCount = 0;

            for(auto& value : values) {
                if(getCharacter(commandIdx) == '\0')
                    break;

                while(getCharacter(commandIdx) == ' ')
                    commandIdx++;

                if(getCharacter(commandIdx) < '0' || getCharacter(commandIdx) > '9') {
                    valid = false;
                    break;
                }

                while(getCharacter(commandIdx) >= '0' && getCharacter(commandIdx) <= '9') {
                    value = value * 10.0f + (getCharacter(commandIdx) - '0');
                    commandIdx++;
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
        std::shared_ptr<IKinematics> kinematics,
        std::shared_ptr<IComms> comms) {
    return std::make_shared<Robot>(stepEngine, endStopX, endStopY, endStopZ, kinematics, comms);
}
