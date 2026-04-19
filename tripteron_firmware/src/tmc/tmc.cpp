#include "tmc.h"

bool Tmc::Enable() {
    mEn->Reset();
    SetSpeed(0);
    return mStep->Start();
}

bool Tmc::Disable() {
    mEn->Set();
    SetSpeed(0);
    return mStep->Stop();
}

void Tmc::SetDirection(bool dir) {
    dir ? mDir->Set() : mDir->Reset();
}

void Tmc::ToggleDirection() {
    mDir->Toggle();
}

void Tmc::SetSpeed(uint32_t freq) {
    mStep->SetFrequency(freq);
}

std::shared_ptr<ITmc> MakeITmc(std::shared_ptr<ITimer> step, std::shared_ptr<IGpioOutput> dir, std::shared_ptr<IGpioOutput> en){
    return std::make_shared<Tmc>(step, dir, en);
}
