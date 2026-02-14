#include "stm32f4xx_hal.h"
#include <string>
#include "utils.h"

class Timer {
public:
    Timer(TIM_HandleTypeDef *htim, uint32_t channel, TimerId id) : mId(id), mHtim(htim), mChannel(channel) {}
    bool Start();
    bool Stop();
    void SetFrecuency(uint32_t freq);

    const TimerId mId;
private:
    TIM_HandleTypeDef *mHtim;
    uint32_t mChannel;
};
