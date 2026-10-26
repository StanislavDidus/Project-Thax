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

    bool STATRegister::Update(PPUMode mode, bool lcyly)
    {
        const bool line =
        (IsLYCIntSelect()   && lcyly) ||
        (IsMode2InSelect()  && mode == PPUMode::MODE2) ||
        (IsMode1InSelect()  && mode == PPUMode::MODE1) ||
        (IsMode0InSelect()  && mode == PPUMode::MODE0);

        const bool rising = line && !m_active;
        m_active = line;
        return rising;
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
}
