#pragma once

#include "CPU/Registers/AByteRegister.hpp"

namespace dmg
{
    [[nodiscard]] inline uint8_t GetColorByID(const AByteRegister& reg, uint8_t id)
    {
        switch (id & 0b11)
        {
        case 0:
            return reg.Read() & 0b11;
            break;
        case 1:
            return (reg.Read() & 0b1100) >> 2;
            break;
        case 2:
            return (reg.Read() & 0b110000) >> 4;
            break;
        case 3:
            return (reg.Read() & 0b11000000) >> 6;
            break;
        default:
            DMG_ERROR("Program should never reach this point. PaletteRegister::GetColorByID()");
            return -1;
        }
    }

}
