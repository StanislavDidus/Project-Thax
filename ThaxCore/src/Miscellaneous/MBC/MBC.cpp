#include "MBC.hpp"

dmg::MBC::MBC(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, const uint8_t components)
    : ROM(ROM)
    , RAM(RAM)
    , m_components(components)
{
}

void dmg::MBC::CheckAllowedComponents(const uint8_t allowed_components) const
{
    if (m_components & ~allowed_components)
    {
        throw std::runtime_error{"Incompatible component was attache to an MBC chip."};
    }
}

dmg::byte dmg::MBC::GetROMSize() const
{
    return byte{static_cast<int>(ROM.size())};
}

dmg::byte dmg::MBC::GetRAMSize() const
{
    return byte{static_cast<int>(RAM.size())};
}

bool dmg::MBC::HasComponent(const uint8_t components) const
{
    return m_components & components;
}

unsigned int dmg::MBC::GetROMBankAddress(unsigned int bank)
{
    return bank * static_cast<int>(globals::ROM_N.m_size);
}
