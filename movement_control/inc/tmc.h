#pragma once

#include "timer.h"
#include "gpio_wrapper.h"
#include <memory>

class TMC {
public:
    TMC(std::shared_ptr<Timer> step, std::shared_ptr<GPIO> dir, std::shared_ptr<GPIO> en)
        : mStep(step), mDir(dir), mEn(en) {}
    bool Enable();
    bool Disable();
    void SetDirection(bool dir);
    void ToggleDirection();
    void SetSpeed(uint32_t freq);
private:
    std::shared_ptr<Timer> mStep;
    std::shared_ptr<GPIO> mDir;
    std::shared_ptr<GPIO> mEn;
};
