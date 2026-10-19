#include "Object.hpp"

#include <stdexcept>

#include "Utils/Math.hpp"

namespace dmg
{
    void Object::Write(const uint16_t address, const uint8_t value)
    {
        if (address == 0) m_y = value;
        if (address == 1) m_x = value;
        if (address == 2) m_tile_index = value;
        if (address == 3) m_attributes = value;

        throw std::runtime_error{"Trying to access bytes outside of the object's memory."};
    }

    uint8_t Object::Read(const uint8_t address) const
    {
        if (address == 0) return m_y;
        if (address == 1) return m_x;
        if (address == 2) return m_tile_index;
        if (address == 3) return m_attributes;

        throw std::runtime_error{"Trying to access bytes outside of the object's memory."};
        return 0;
    }

    uint8_t Object::GetY() const
    {
        return m_y;
    }

    uint8_t Object::GetX() const
    {
        return m_x;
    }

    uint8_t Object::GetTileIndex() const
    {
        return m_tile_index;
    }

    uint8_t Object::GetAttributes() const
    {
        return m_attributes;
    }

    bool Object::GetPriority() const
    {
        return getBit(m_attributes, 7);
    }

    bool Object::IsYFlip() const
    {
        return getBit(m_attributes, 6);
    }

    bool Object::IsXFlip() const
    {
        return getBit(m_attributes, 5);
    }

    ObjectPalette Object::GetDMGPalette() const
    {
        return getBit(m_attributes, 4) == 0 ? ObjectPalette::OBP0 : ObjectPalette::OBP1;
    }
}
