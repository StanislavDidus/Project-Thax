#pragma once

#include "MBC.hpp"

namespace dmg
{
    class MBC1 : public MBC
    {
    public:
        MBC1(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, uint8_t components);
        ~MBC1() override = default;

        void WriteROM(uint16_t address, uint8_t value) override;
        void WriteRAM(uint16_t address, uint8_t value) override;
        [[nodiscard]] uint8_t ReadROM(uint16_t address) const override;
        [[nodiscard]] uint8_t ReadRAM(uint16_t address) const override;
    private:
        void SetBank1(int value);
        [[nodiscard]] uint8_t GetROMMask() const;

        void CheckIfMultiCart();

        bool m_ram_enabled = false;
        bool m_multicart = true;
        uint8_t m_bank1 = 1; // First 5 bits of the bank switcher
        uint8_t m_bank2 = 0; // Next 2 bits
        uint8_t m_mode = 0;
    };
}