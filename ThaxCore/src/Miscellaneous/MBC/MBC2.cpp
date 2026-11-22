#include "MBC2.hpp"

namespace dmg
{
    dmg::MBC2::MBC2(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, const uint8_t components)
        : MBC{ROM, RAM, components}
    {
        if (GetROMSize() > 256_kib || GetRAMSize() != 0_b)
        {
            throw std::runtime_error{"Incompatible rom or ram size for this MBC."};
        }

        CheckAllowedComponents(C_BATTERY);
    }

    void dmg::MBC2::WriteROM(const uint16_t address, const uint8_t value)
    {
        if (globals::MBC2_RAM_ENABLE.Contains(address))
        {
            if (GetBit(address, 8) == 0)
            {
                m_ram_enabled = (value & 0xF) == 0xA;
            }
            else
            {
                uint8_t bank = value & 0xF;
                if (bank == 0) bank = 1;
                m_rom_bank = bank & GetROMMask();
            }
        }
    }

    void dmg::MBC2::WriteRAM(const uint16_t address, const uint8_t value)
    {
        if (!m_ram_enabled) return;

        m_half_ram[address & 0x1FF] = value & 0xF;
    }

    uint8_t dmg::MBC2::ReadROM(const uint16_t address) const
    {
        if (globals::ROM_0.Contains(address))
        {
            return ROM[address];
        }
        if (globals::ROM_N.Contains(address))
        {
            return ROM[GetROMBankAddress(m_rom_bank) - 1 * globals::ROM_N.m_size + address];
        }

        return globals::GARBAGE;
    }

    uint8_t dmg::MBC2::ReadRAM(const uint16_t address) const
    {
        if (!m_ram_enabled)
            return globals::GARBAGE;

        return 0xF0 | (m_half_ram[address & 0x1FF] & 0xF);
    }
}