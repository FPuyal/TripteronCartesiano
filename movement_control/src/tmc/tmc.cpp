#include "tmc.h"

bool TMC::Enable() {
    mEn->Reset();
    SetSpeed(0);
    return mStep->Start();
}

bool TMC::Disable() {
    mEn->Set();
    SetSpeed(0);
    return mStep->Stop();
}

void TMC::SetDirection(bool dir) {
    dir ? mDir->Set() : mDir->Reset();
}

void TMC::ToggleDirection() {
    mDir->Toggle();
}

void TMC::SetSpeed(uint32_t freq) {
    mStep->SetFrecuency(freq);
}
