#include "MBC2.hpp"

dmg::MBC2::MBC2(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, const uint8_t components)
    : MBC{ROM, RAM, components}
{
    if (GetROMSize() > 256_kib || GetRAMSize() != 0_b)
    {
        throw std::runtime_error{"Incompatible rom or ram size for this MBC."};
    }

    CheckAllowedComponents(C_BATTERY);
}

void dmg::MBC2::WriteROM(uint16_t address, uint8_t value)
{
}

void dmg::MBC2::WriteRAM(uint16_t address, uint8_t value)
{
}

uint8_t dmg::MBC2::ReadROM(uint16_t address) const
{
    return globals::GARBAGE;
}

uint8_t dmg::MBC2::ReadRAM(uint16_t address) const
{
    return globals::GARBAGE;
}
