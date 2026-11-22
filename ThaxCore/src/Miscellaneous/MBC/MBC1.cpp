#include "MBC1.hpp"

namespace dmg
{
    MBC1::MBC1(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, const uint8_t components)
        : MBC(ROM, RAM, components)
    {
        if (GetROMSize() > 2_mib || GetRAMSize() > 32_kib)
        {
            throw std::runtime_error{"Incompatible rom or ram size for this MBC."};
        }

        CheckAllowedComponents(C_RAM | C_BATTERY);

        CheckIfMultiCart();
    }

    void MBC1::WriteROM(const uint16_t address, const uint8_t value)
    {
        if (globals::MBC1_RAM_ENABLE.Contains(address))
        {
            m_ram_enabled = (value & 0xF) == 0xA;
        }
        else if (globals::MBC1_ROM_BANK_NUMBER.Contains(address))
        {
            SetBank1(value);
        }
        else if (globals::MBC1_RAM_BANK_NUMBER.Contains(address))
        {
            m_bank2 = value & 0b11;
        }
        else if (globals::MBC1_BANKING_MODE_SELECT.Contains(address))
        {
            m_mode = value & 0b1;
        }
    }

    void MBC1::WriteRAM(const uint16_t address, const uint8_t value)
    {
        if (!HasComponent(C_RAM) || !m_ram_enabled)
            return;

        if (GetRAMSize() == 8_kib || m_mode == 0)
        {
            RAM[address] = value;
            return;
        }

        RAM[address + m_bank2 * globals::EXTERNAL_RAM.m_size] = value;
    }

    uint8_t MBC1::ReadROM(const uint16_t address) const
    {
        if (globals::ROM_0.Contains(address))
        {
            if (m_mode == 0)
                return ROM[address];
            if (m_mode == 1)
            {
                if (m_multicart)
                    return ROM[GetROMBankAddress((m_bank2 << 4) & GetROMMask()) + address];
                else
                    return ROM[GetROMBankAddress((m_bank2 << 5) & GetROMMask()) + address];
            }

        }
        if (globals::ROM_N.Contains(address))
        {
            if (m_multicart)
                return ROM[GetROMBankAddress(((m_bank2 << 4) | m_bank1) & GetROMMask()) - 1 * globals::ROM_N.m_size + address];
            else
                return ROM[GetROMBankAddress(((m_bank2 << 5) | m_bank1) & GetROMMask()) - 1 * globals::ROM_N.m_size + address];
        }

        return globals::GARBAGE;
    }

    uint8_t MBC1::ReadRAM(const uint16_t address) const
    {
        if (!HasComponent(C_RAM) || !m_ram_enabled)
            return globals::GARBAGE;

        if (GetRAMSize() == 8_kib || m_mode == 0)
            return RAM[address];

        if (m_mode == 1)
            return RAM[address + m_bank2 * globals::EXTERNAL_RAM.m_size];

        return globals::GARBAGE;
    }

    void MBC1::SetBank1(const int value)
    {
        uint8_t five_bit_register = value & 0x1F;
        if (five_bit_register == 0) five_bit_register = 1;
        if (m_multicart)
            m_bank1 = five_bit_register & 0xF;
        else
            m_bank1 = five_bit_register;
    }

    void MBC1::CheckIfMultiCart()
    {
        if (GetROMSize() != 1_mib)
        {
            m_multicart = false;
            return;
        }

        bool bank_10 = true;
        for (int i = 0x104; i < 0x133; ++i)
        {
            if (ROM[GetROMBankAddress(0x10) + i] != ROM[GetROMBankAddress(0x00) + i])
            {
                bank_10 = false;
                break;
            }
        }
        bool bank_20 = true;
        for (int i = 0x104; i < 0x133; ++i)
        {
            if (ROM[GetROMBankAddress(0x20) + i] != ROM[GetROMBankAddress(0x00) + i])
            {
                bank_20 = false;
                break;
            }
        }
        bool bank_30 = true;
        for (int i = 0x104; i < 0x133; ++i)
        {
            if (ROM[GetROMBankAddress(0x30) + i] != ROM[GetROMBankAddress(0x00) + i])
            {
                bank_30 = false;
                break;
            }
        }

        if (!bank_10 && !bank_20 && !bank_30)
            m_multicart = false;
    }
}
