#include "MBC1.hpp"

namespace dmg
{
    MBC1::MBC1(const CARTRIDGE_TYPE cartridge_type, const ROM_SIZE rom_size, const RAM_SIZE ram_size)
        : MBC(cartridge_type, rom_size, ram_size)
    {
    }

    void MBC1::WriteROM(uint16_t address, uint8_t value)
    {
        if (globals::RAM_ENABLE.Contains(address))
        {
            m_ram_enabled = (value & 0xF) == 0xA;
        }
        else if (globals::ROM_BANK_NUMBER.Contains(address))
        {
            switch (GetROMSize())
            {
            case ROM_SIZE::BANKS_2:
                SetActiveROMBank(1);
                break;
            case ROM_SIZE::BANKS_4:
                SelectROMBank(value, 0b11);
                break;
            case ROM_SIZE::BANKS_8:
                SelectROMBank(value, 0b111);
                break;
            case ROM_SIZE::BANKS_16:
                SelectROMBank(value, 0b1111);
                break;
            case ROM_SIZE::BANKS_32:
                SelectROMBank(value, 0b11111);
                break;
            case ROM_SIZE::BANKS_64:
                SelectROMBank(value, 0b111111);
                break;
            case ROM_SIZE::BANKS_128:
                SelectROMBank(value, 0b1111111);
                break;
            default: ;
            }

        }
        else if (globals::RAM_BANK_NUMBER.Contains(address))
        {
            SetActiveRAMBank(value & 0b11);
        }
        else if (globals::BANKING_MODE_SELECT.Contains(address))
        {
            if (!(static_cast<int>(GetROMSize()) <= static_cast<int>(ROM_SIZE::BANKS_32) && static_cast<int>(GetRAMSize()) <= static_cast<int>(RAM_SIZE::BANK_1)))
            {
                m_banking_mode = value & 0b1;
            }
        }
    }

    void MBC1::WriteRAM(const uint16_t address, const uint8_t value)
    {
        if (m_ram_enabled)
            RAM[address] = value;
    }

    uint8_t MBC1::ReadRAM(const uint16_t address) const
    {
        if (!m_ram_enabled)
            return 0xFF;

        return RAM[address];
    }

    void MBC1::SelectROMBank(const int value, const uint8_t mask)
    {
        const uint8_t five_bit_register = value & 0x1F;
        if (mask > 0b11111)
        {
            // Use additional two bits if mask requires more than 5 bits
            SetActiveROMBank(five_bit_register == 0 ? 1 : ((GetActiveROMBank() << 5) | five_bit_register) & mask);
        }
        else
        {
            SetActiveROMBank(five_bit_register == 0 ? 1 : five_bit_register & mask);
        }
    }
}
