#include "Object.hpp"

#include <stdexcept>

#include "Utils/Math.hpp"

namespace dmg
{
    Object::Object(const uint16_t address, const std::span<uint8_t> oam)
        : m_oam(oam)
        , m_address(address)
    {
    }

    uint8_t Object::GetYPosition() const
    {
        return m_oam[m_address];
    }

    uint8_t Object::GetXPosition() const
    {
        return m_oam[m_address + 1];
    }

    uint8_t Object::GetTileIndex() const
    {
        return m_oam[m_address + 2];
    }

    bool Object::GetPriority() const
    {
        return getBit(m_oam[m_address + 3], 7);
    }

    bool Object::IsYFlipped() const
    {
        return getBit(m_oam[m_address + 3], 6);
    }

    bool Object::IsXFlipped() const
    {
        return getBit(m_oam[m_address + 3], 5);
    }

    ObjectPalette Object::GetPalette() const
    {
        return getBit(m_oam[m_address + 3], 4) == 0 ? ObjectPalette::OBP0 : ObjectPalette::OBP1;
    }
}
