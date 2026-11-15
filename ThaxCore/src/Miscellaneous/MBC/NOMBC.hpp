#pragma once

#include "MBC.hpp"

namespace dmg
{
    class NOMBC : public MBC
    {
    public:
        NOMBC(CARTRIDGE_TYPE cartridge_type, ROM_SIZE rom_size, RAM_SIZE ram_size);
        ~NOMBC() override = default;

        void WriteROM(uint16_t address, uint8_t value) override;
        void WriteRAM(uint16_t address, uint8_t value) override;
        [[nodiscard]] uint8_t ReadRAM(uint16_t address) const override;
    };
}
