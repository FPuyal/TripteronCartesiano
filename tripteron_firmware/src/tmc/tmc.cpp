#include "tmc.h"
#include "usart.h"

Tmc::Tmc(std::shared_ptr<ITimer> step, std::shared_ptr<IGpioOutput> dir, std::shared_ptr<IGpioOutput> en, std::shared_ptr<IUart> uart, uint16_t microSteps, uint8_t nodeAddr)
    : mStep(step), mDir(dir), mEn(en), mUart(uart) {
        ConfigureRegisters(microSteps, nodeAddr);
    }

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

void Tmc::ConfigureRegisters(uint16_t microSteps, uint8_t nodeAddr) {
    uint8_t mres;

    switch (microSteps) {
        case 256: mres = 0x10; break;
        case 128: mres = 0x11; break;
        case 64:  mres = 0x12; break;
        case 32:  mres = 0x13; break;
        case 16:  mres = 0x14; break;
        case 8:   mres = 0x15; break;
        case 4:   mres = 0x16; break;
        case 2:   mres = 0x17; break;
        case 1:   mres = 0x18; break;
        default:  mres = 0x18; break;
    }
    // 1. GCONF — SpreadCycle + control por UART
    // Cambio: bit2 (en_SpreadCycle) = 1
    // Valor: 0x000000C4
    uint8_t gconf[8] = {0x05, nodeAddr, 0x80, 0x00, 0x00, 0x00, 0xC4, 0x00};
    gconf[7] = tmc_crc8(gconf, 7);
    mUart->WriteData(gconf);

    // 2. IHOLD_IRUN — IRUN=20 (~65%), IHOLD=8, IHOLDDELAY=6
    // Reduce calentamiento sin sacrificar torque crítico
    // Valor: 0x00061408
    uint8_t ihold[8] = {0x05, 0x00, 0x90, 0x00, 0x06, 0x14, 0x08, 0x00};
    ihold[7] = tmc_crc8(ihold, 7);
    mUart->WriteData(ihold);

    // 3. TPOWERDOWN — sin cambio
    uint8_t tpdown[8] = {0x05, 0x00, 0x91, 0x00, 0x00, 0x00, 0x14, 0x00};
    tpdown[7] = tmc_crc8(tpdown, 7);
    mUart->WriteData(tpdown);

    // 4. CHOPCONF — 1/4 micropasos, TBL=1, TOFF=4, HSTRT=4, HEND=1, intpol=1
    // MRES=6 → 1/4 microsteps
    // Valor: 0x160080C4
    uint8_t chopconf[8] = {0x05, 0x00, 0xEC, mres, 0x00, 0x80, 0xC4, 0x00};
    chopconf[7] = tmc_crc8(chopconf, 7);
    mUart->WriteData(chopconf);
}

std::shared_ptr<ITmc> MakeITmc(std::shared_ptr<ITimer> step, std::shared_ptr<IGpioOutput> dir, std::shared_ptr<IGpioOutput> en, std::shared_ptr<IUart> uart, uint16_t microSteps, uint8_t nodeAddr) {
    return std::make_shared<Tmc>(step, dir, en, uart, microSteps, nodeAddr);
}
