#pragma once
#include <cstdint>

#include "ATwoByteRegister.hpp"

namespace dmg
{
    struct Address
    {
        explicit Address(uint16_t address) : m_address(address) {}
        explicit Address(const ATwoByteRegister& reg) : m_address(reg.ReadWord()) {}

        uint16_t m_address = 0;
    };
}
