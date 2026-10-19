#include "STATRegister.hpp"

#include "Utils/Math.hpp"

namespace dmg
{
    uint8_t STATRegister::Read() const
    {
        return m_data;
    }

    void STATRegister::Write(uint8_t data)
    {
        m_data = data & 0b11111000;
    }

    bool STATRegister::IsLYCIntSelect() const
    {
        return getBit(m_data, 6);
    }

    bool STATRegister::IsMode2InSelect() const
    {
        return getBit(m_data, 5);
    }

    bool STATRegister::IsMode1InSelect() const
    {
        return getBit(m_data, 4);
    }

    bool STATRegister::IsMode0InSelect() const
    {
        return getBit(m_data, 3);
    }

    bool STATRegister::isLYCeqLC() const
    {
        return getBit(m_data, 2);
    }

    PPUMode STATRegister::getPPUMode() const
    {
        switch (m_data & 0b11)
        {
        case 0b00: return PPUMode::MODE0;
        case 0b01: return PPUMode::MODE1;
        case 0b10: return PPUMode::MODE2;
        case 0b11: return PPUMode::MODE3;
        default: return PPUMode::MODE0;
        }
    }
}
