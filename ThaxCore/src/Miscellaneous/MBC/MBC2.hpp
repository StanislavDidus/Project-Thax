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
        std::array<uint8_t, globals::MBC2_RAM_SIZE> m_half_ram {};

        bool m_ram_enabled = false;
        uint8_t m_rom_bank = 1;
    };
}
