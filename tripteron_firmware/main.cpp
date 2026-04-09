#include "hardware_manager_interface.h"
#include "stm32f4xx_hal.h"
#include "trajectory_generator_utils.h"
#include "utils.h"

#include "trajectory_generator_interface.h"

#include <memory>

#include "usart.h"

// CRC-8 para TMC2209
static uint8_t tmc_crc8(uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        for (uint8_t b = 0; b < 8; b++) {
            if ((crc >> 7) ^ (byte & 0x01))
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
            byte >>= 1;
        }
    }
    return crc;
}

int main(){

    auto hardwareManager = MakeIHardwareManager();

    hardwareManager->InitHardware();

    MX_USART2_UART_Init();
    HAL_Delay(500);

    HAL_StatusTypeDef status;

    // 1. GCONF — SpreadCycle + control por UART
    // Cambio: bit2 (en_SpreadCycle) = 1
    // Valor: 0x000000C4
    uint8_t gconf[8] = {0x05, 0x00, 0x80, 0x00, 0x00, 0x00, 0xC4, 0x00};
    gconf[7] = tmc_crc8(gconf, 7);
    HAL_UART_Transmit(&huart2, gconf, 8, HAL_MAX_DELAY);
    HAL_Delay(10);

    // 2. IHOLD_IRUN — IRUN=20 (~65%), IHOLD=8, IHOLDDELAY=6
    // Reduce calentamiento sin sacrificar torque crítico
    // Valor: 0x00061408
    uint8_t ihold[8] = {0x05, 0x00, 0x90, 0x00, 0x06, 0x14, 0x08, 0x00};
    ihold[7] = tmc_crc8(ihold, 7);
    HAL_UART_Transmit(&huart2, ihold, 8, HAL_MAX_DELAY);
    HAL_Delay(10);

    // 3. TPOWERDOWN — sin cambio
    uint8_t tpdown[8] = {0x05, 0x00, 0x91, 0x00, 0x00, 0x00, 0x14, 0x00};
    tpdown[7] = tmc_crc8(tpdown, 7);
    HAL_UART_Transmit(&huart2, tpdown, 8, HAL_MAX_DELAY);
    HAL_Delay(10);

    // 4. CHOPCONF — 1/4 micropasos, TBL=1, TOFF=4, HSTRT=4, HEND=1, intpol=1
    // MRES=6 → 1/4 microsteps
    // Valor: 0x160080C4
    uint8_t chopconf[8] = {0x05, 0x00, 0xEC, 0x16, 0x00, 0x80, 0xC4, 0x00};
    chopconf[7] = tmc_crc8(chopconf, 7);
    HAL_UART_Transmit(&huart2, chopconf, 8, HAL_MAX_DELAY);
    HAL_Delay(10);

    auto tmcX = hardwareManager->GetTmcs()[TmcId::TMCX];
    tmcX->Enable();

    TrajectoryConfig config {
        12000,
        1600,
        16000
       };

    auto trajectoryGenerator = MakeITrajectoryGenerator(config);

    MotionState initState {0.0, 0.0};
    MotionState finalState {100000, config.velMax};

    trajectoryGenerator->SetTrajectoryProfile(initState, finalState);

    int profileType = trajectoryGenerator->GetProfileType();

    while (1) {
        while (!trajectoryGenerator->IsFinished()) {
            trajectoryGenerator->Update(0.001); // Update every 10 ms

            double velocity = trajectoryGenerator->GetVelocity();

            tmcX->SetSpeed(static_cast<uint32_t>(velocity));

            HAL_Delay(1); // Delay for 10 ms
        }
    }
}
