#pragma once

#include <vector>

#include "MBC.hpp"

namespace dmg
{
    class MBC2 : public MBC
    {
    public:
        MBC2(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, uint8_t components);
        ~MBC2() override = default;

        void WriteROM(uint16_t address, uint8_t value) override;
        void WriteRAM(uint16_t address, uint8_t value) override;
        [[nodiscard]] uint8_t ReadROM(uint16_t address) const override;
        [[nodiscard]] uint8_t ReadRAM(uint16_t address) const override;
    private:
        bool m_ram_enabled = false;
        uint8_t m_two_bits_bank = 0;
        uint8_t m_banking_mode = 0;
    };
}
