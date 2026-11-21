#include "NOMBC.hpp"

namespace dmg
{
    NOMBC::NOMBC(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM)
        : MBC(ROM, RAM, C_NONE)
    {
    }

    void NOMBC::WriteROM(uint16_t address, uint8_t value)
    {
    }

    void NOMBC::WriteRAM(const uint16_t address, const uint8_t value)
    {
    }

    uint8_t NOMBC::ReadROM(uint16_t address) const
    {
        return ROM[address];
    }

    uint8_t NOMBC::ReadRAM(const uint16_t address) const
    {
        return RAM[address];
    }

}
