#pragma once
#include <vector>

#include "MBC.hpp"

namespace dmg
{
    struct RTC
    {
        uint8_t s  = 0;
        uint8_t m  = 0;
        uint8_t h  = 0;
        uint8_t dl = 0;
        uint8_t dh = 0;
    };

    class MBC3 : public MBC
    {
    public:
        MBC3(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, uint8_t components);
        ~MBC3() override = default;

        [[nodiscard]] uint8_t ReadRAM(uint16_t address) const override;
        [[nodiscard]] uint8_t ReadROM(uint16_t address) const override;
        void WriteRAM(uint16_t address, uint8_t value) override;
        void WriteROM(uint16_t address, uint8_t value) override;
    private:
        bool m_ram_enabled = false;
        int m_rom_bank = 1;
        int m_ram_bank_number = 0;
        int latch_clock = 0;

        RTC m_rtc {};
        RTC m_rtc_offset {};
    };

}
