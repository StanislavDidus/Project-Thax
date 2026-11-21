#pragma once

#include "MBC.hpp"

namespace dmg
{
    class NOMBC : public MBC
    {
    public:
        NOMBC(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM);
        ~NOMBC() override = default;

        void WriteROM(uint16_t address, uint8_t value) override;
        void WriteRAM(uint16_t address, uint8_t value) override;
        [[nodiscard]] uint8_t ReadROM(uint16_t address) const override;
        [[nodiscard]] uint8_t ReadRAM(uint16_t address) const override;
    };
}
