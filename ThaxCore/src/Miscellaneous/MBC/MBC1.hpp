#pragma once

#include "MBC.hpp"

namespace dmg
{
    class MBC1 : public MBC
    {
    public:
        MBC1(CARTRIDGE_TYPE cartridge_type, ROM_SIZE rom_size, RAM_SIZE ram_size);
        ~MBC1() override = default;

        void WriteROM(uint16_t address, uint8_t value) override;
        void WriteRAM(uint16_t address, uint8_t value) override;
        [[nodiscard]] uint8_t ReadRAM(uint16_t address) const override;
    private:
        void SelectROMBank(int value, uint8_t mask);

        bool m_ram_enabled = false;
        uint8_t m_two_bits_bank = 0;
        uint8_t m_banking_mode = 0;
    };
}