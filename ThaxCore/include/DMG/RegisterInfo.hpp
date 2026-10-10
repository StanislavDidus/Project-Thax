#pragma once

#include <cstdint>

namespace dmg
{
    struct RegisterInfo
    {
        uint8_t F, A, B, C, D, E, H, L;
        uint16_t PC, SP;
    };
}