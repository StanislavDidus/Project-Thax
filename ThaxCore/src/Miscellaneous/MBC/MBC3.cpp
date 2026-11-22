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

    uint8_t MBC3::ReadRAM(uint16_t address) const
    {
        if (m_ram_bank_number < 0x8) // RAM
        {
            return RAM[address + m_ram_bank_number * globals::EXTERNAL_RAM.m_size];
        }
        else if (m_ram_bank_number > 0x7 && m_ram_bank_number < 0xD) // RTC
        {
            switch (m_ram_bank_number)
            {
            case 0x8: return m_rtc_offset.s;
            case 0x9: return m_rtc_offset.m;
            case 0xA: return m_rtc_offset.h;
            case 0xB: return m_rtc_offset.dl;
            case 0xC: return m_rtc_offset.dh;
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
        if (m_ram_bank_number < 0x8) // RAM
        {
            RAM[address + m_ram_bank_number * globals::EXTERNAL_RAM.m_size] = value;
        }
        else if (m_ram_bank_number > 0x7 && m_ram_bank_number < 0xD) // RTC
        {
            switch (m_ram_bank_number)
            {
            case 0x8:
                m_rtc_offset.s = value;
                break;
            case 0x9:
                m_rtc_offset.m = value;
                break;
            case 0xA:
                m_rtc_offset.h = value;
                break;
            case 0xB:
                m_rtc_offset.dl = value;
                break;
            case 0xC:
                m_rtc_offset.dh = value;
                break;
            default: ;
            }
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
            m_rom_bank = value & 0x7F;
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
                auto now = std::chrono::system_clock::now();
                auto today = floor<std::chrono::days>(now);
                std::chrono::hh_mm_ss time{floor<std::chrono::seconds>(now - today)};

                m_rtc.s = time.seconds().count() - m_rtc_offset.s;
                m_rtc.m = time.minutes().count() - m_rtc_offset.m;
                m_rtc.h = time.hours().count() - m_rtc_offset.h;

                const uint16_t day = today.time_since_epoch().count() - (m_rtc_offset.dl | ((m_rtc_offset.dh & 0b1) << 8));
                m_rtc.dl = day & 0xFF;
                m_rtc.dh = day & 0x100;
            }
        }
    }
}
