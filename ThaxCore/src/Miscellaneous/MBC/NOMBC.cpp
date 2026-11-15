#include "NOMBC.hpp"

namespace dmg
{
    NOMBC::NOMBC(const CARTRIDGE_TYPE cartridge_type, const ROM_SIZE rom_size, const RAM_SIZE ram_size)
        : MBC(cartridge_type, rom_size, ram_size)
    {
    }

    void NOMBC::WriteROM(uint16_t address, uint8_t value)
    {
    }

    void NOMBC::WriteRAM(const uint16_t address, const uint8_t value)
    {
        RAM[address] = value;
    }

    uint8_t NOMBC::ReadRAM(const uint16_t address) const
    {
        return RAM[address];
    }
}
