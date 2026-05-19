#pragma once

#include "gpio_input_interface.h"
#include "tmc_interface.h"
#include "trajectory_generator_interface.h"
#include "trajectory_generator_utils.h"
#include "axis_types.h"
#include <memory>
#include <vector>

class IAxis {
public:
    virtual ~IAxis() = default;
    virtual void Tick() = 0;
    virtual bool SetSegment(const std::vector<MotionState>& segment) = 0;
    virtual bool RequestState(AxisCommandRequest command) = 0;
    virtual void EmergencyStop() = 0;

    virtual AxisState GetState() const = 0;
    virtual double GetPosition() const = 0;
    virtual double GetVelocity() const = 0;
};

std::shared_ptr<IAxis> MakeIAxis(std::shared_ptr<IGpioInput> gpioInput, std::shared_ptr<ITmc> tmc, const AxisConfig& config);
