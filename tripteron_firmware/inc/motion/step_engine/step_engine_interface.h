#pragma once

class IStepEngine {
public:
    virtual ~IStepEngine() = default;
    virtual void Update(int16_t steps[3]) = 0;
};

