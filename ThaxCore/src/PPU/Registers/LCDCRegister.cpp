#include "Utils/Math.hpp"
#include "LCDCRegister.hpp"
#include "InternalGlobals.hpp"

namespace dmg
{
    uint8_t LCDCRegister::Read() const
    {
        return m_data;
    }

    void LCDCRegister::Write(const uint8_t data)
    {
        m_data = data;
    }

    bool LCDCRegister::IsLCDEnabled() const
    {
        return getBit(m_data, 7);
    }

    uint16_t LCDCRegister::GetWindowTileMapArea() const
    {
        if (getBit(m_data, 6) == 0)
        {
            return globals::TILEMAP_AREA_1.m_start;
        }
        else
        {
            return globals::TILEMAP_AREA_2.m_start;
        }
    }

    bool LCDCRegister::IsWindowEnabled() const
    {
        return getBit(m_data, 5);
    }

    uint16_t LCDCRegister::GetBGWindowTileDataArea() const
    {
        if (getBit(m_data, 4) == 0)
        {
            return globals::TILE_BLOCK_1.m_start;
        }
        else
        {
            return globals::TILE_BLOCK_2.m_start;
        }
    }

    uint16_t LCDCRegister::GetBGTileMapArea() const
    {
        if (getBit(m_data, 3) == 0)
        {
            return globals::TILEMAP_AREA_1.m_start;
        }

        return globals::TILEMAP_AREA_2.m_start;
    }

    ObjectSize LCDCRegister::GetObjectSize() const
    {
        if (getBit(m_data, 2) == 0)
        {
            return ObjectSize::SIZE_8X8;
        }
        else
        {
            return ObjectSize::SIZE_8X16;
        }
    }

    bool LCDCRegister::AreObjectsEnabled() const
    {
        return getBit(m_data, 1);
    }

    bool LCDCRegister::IsBGWindowPriorityEnabled() const
    {
        return getBit(m_data, 0);
    }
}
