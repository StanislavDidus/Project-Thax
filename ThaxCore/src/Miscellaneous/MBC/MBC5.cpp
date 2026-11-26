#include "MBC5.hpp"

#include "Precompiled.hpp"

namespace dmg
{
    MBC5::MBC5(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, const uint8_t components)
        : MBC(ROM, RAM, components)
    {
        if (GetRAMSize() == 64_kib)
        {
            throw std::runtime_error{"Incompatible ram size for this MBC."};
        }

        CheckAllowedComponents(C_RAM | C_BATTERY | C_RUMBLE);
    }

    void MBC5::WriteROM(const uint16_t address, const uint8_t value)
    {
        if (globals::MBC1_RAM_ENABLE.Contains(address))
        {
            if (value == 0x0A) m_ram_enabled = true;
            else if (value == 0x0) m_ram_enabled = false;
        }
        else if (globals::MBC1_ROM_BANK_NUMBER.Contains(address))
        {
            m_bank1 = value;
        }
        else if (globals::MBC1_RAM_BANK_NUMBER.Contains(address))
        {
            m_bank2 = value & 0x1;
        }
        else if (globals::MBC1_BANKING_MODE_SELECT.Contains(address))
        {
            m_ram_bank = value & 0xF;
        }
    }

    void MBC5::WriteRAM(const uint16_t address, const uint8_t value)
    {
        if (!HasComponent(C_RAM) || !m_ram_enabled)
            return;

        RAM[address + m_ram_bank * globals::EXTERNAL_RAM.m_size] = value;
    }

    uint8_t MBC5::ReadROM(const uint16_t address) const
    {
        if (globals::ROM_0.Contains(address))
        {
            return ROM[address];

        }
        if (globals::ROM_N.Contains(address))
        {
            return ROM[GetROMBankAddress(((m_bank2 << 8) | m_bank1) & GetROMMask()) - 1 * globals::ROM_N.m_size + address];
        }

        return globals::GARBAGE;
    }

    uint8_t MBC5::ReadRAM(const uint16_t address) const
    {
        if (!HasComponent(C_RAM) || !m_ram_enabled)
            return globals::GARBAGE;

        return RAM[address + m_ram_bank * globals::EXTERNAL_RAM.m_size];
    }
}
