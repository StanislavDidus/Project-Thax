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

        RTC operator+(const RTC& other) const
        {
            RTC rtc;
            rtc.s = this->s + other.s;
            rtc.m = this->m + other.m;
            rtc.h = this->h + other.h;
            rtc.dl = this->dl + other.dl;
            rtc.dh = this->dh + other.dh;
            return rtc;
        }

        RTC operator-(const RTC& other) const
        {
            RTC rtc;
            rtc.s = this->s - other.s;
            rtc.m = this->m - other.m;
            rtc.h = this->h - other.h;
            rtc.dl = this->dl - other.dl;
            rtc.dh = this->dh - other.dh;
            return rtc;
        }

        void operator+=(const RTC& other)
        {
            *this = *this + other;
        }

        void operator-=(const RTC& other)
        {
            *this = *this - other;
        }
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
        [[nodiscard]] RTC GetNowRTC() const;
        void Halt();
        void UnHault();

        bool m_ram_enabled = false;
        int m_rom_bank = 1;
        int m_ram_bank_number = 0;
        int latch_clock = 0;

        bool m_halted = false;
        int m_carry = 0;

        RTC m_rtc {};
        RTC m_rtc_base {};
        RTC m_rtc_halted {};
    };

}
