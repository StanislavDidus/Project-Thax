#include "Precompiled.hpp"
#include "MBC3.hpp"

namespace dmg
{
    MBC3::MBC3(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, uint8_t components)
        : MBC{ROM, RAM, components}
    {
        if (GetROMSize() > 4_mib || GetRAMSize() > 64_kib)
        {
            throw std::runtime_error{"Incompatible rom or ram size for this MBC."};
        }

        CheckAllowedComponents(C_RAM | C_BATTERY | C_TIMER);
    }

    uint8_t MBC3::ReadRAM(const uint16_t address) const
    {
        if (!m_ram_enabled) return globals::GARBAGE;

        if (m_ram_bank_number < 0x8) // RAM
        {
            return RAM[address + m_ram_bank_number * globals::EXTERNAL_RAM.m_size];
        }
        else if (m_ram_bank_number > 0x7 && m_ram_bank_number < 0xD) // RTC
        {
            switch (m_ram_bank_number)
            {
            case 0x8: return m_rtc.s;
            case 0x9: return m_rtc.m;
            case 0xA: return m_rtc.h;
            case 0xB: return m_rtc.dl;
            case 0xC: return m_rtc.dh;
            default: ;
            }
        }

        return globals::GARBAGE;
    }

    uint8_t MBC3::ReadROM(uint16_t address) const
    {
        if (globals::ROM_0.Contains(address))
        {
            return ROM[address];
        }
        if (globals::ROM_N.Contains(address))
        {
            return ROM[GetROMBankAddress(m_rom_bank & GetROMMask()) - 1 * globals::ROM_N.m_size + address];
        }

        return globals::GARBAGE;
    }

    void MBC3::WriteRAM(const uint16_t address, const uint8_t value)
    {
        if (!m_ram_enabled) return;

        if (m_ram_bank_number < 0x8) // RAM
        {
            RAM[address + m_ram_bank_number * globals::EXTERNAL_RAM.m_size] = value;
        }
        else if (m_ram_bank_number > 0x7 && m_ram_bank_number < 0xD) // RTC
        {
            RTC rtc = GetNowRTC() - m_rtc_base;
            switch (m_ram_bank_number)
            {
            case 0x8:
                rtc.s = value;
                break;
            case 0x9:
                rtc.m = value;
                break;
            case 0xA:
                rtc.h = value;
                break;
            case 0xB:
                rtc.dl = value;
                break;
            case 0xC:
                rtc.dh = value;
                break;
            default: ;
            }

            m_rtc_base = GetNowRTC() - rtc;
        }
    }

    void MBC3::WriteROM(const uint16_t address, const uint8_t value)
    {
        if (globals::MBC1_RAM_ENABLE.Contains(address))
        {
            m_ram_enabled = (value & 0xF) == 0xA;
        }
        else if (globals::MBC1_ROM_BANK_NUMBER.Contains(address))
        {
            m_rom_bank = value & 0xFF;
            if (m_rom_bank == 0) m_rom_bank = 1;
        }
        else if (globals::MBC1_RAM_BANK_NUMBER.Contains(address))
        {
            m_ram_bank_number = value;
        }
        else if (globals::MBC1_BANKING_MODE_SELECT.Contains(address))
        {
            if (latch_clock == 0 && value == 1)
            {
                m_rtc = GetNowRTC() - m_rtc_base;
            }

            latch_clock = value;
        }
    }

    RTC MBC3::GetNowRTC() const
    {
        RTC rtc {};

        auto now = std::chrono::system_clock::now();
        auto today = floor<std::chrono::days>(now);
        std::chrono::hh_mm_ss time{floor<std::chrono::seconds>(now - today)};

        rtc.s = time.seconds().count() - m_rtc_base.s;
        rtc.m = time.minutes().count() - m_rtc_base.m;
        rtc.h = time.hours().count() - m_rtc_base.h;

        const uint16_t day = today.time_since_epoch().count() - (m_rtc_base.dl | ((m_rtc_base.dh & 0b1) << 8));
        rtc.dl = day & 0xFF;
        rtc.dh = ((day & 0x100) >> 8) | (m_halted << 6) | (m_carry << 7);

        return rtc;
    }

    void MBC3::Halt()
    {
        m_halted = true;
        m_rtc_halted = GetNowRTC();
    }

    void MBC3::UnHault()
    {
        m_halted = false;
        m_rtc_base -= GetNowRTC() - m_rtc_halted;
    }
}
