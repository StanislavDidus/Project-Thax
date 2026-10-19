#pragma once

#include <cstdint>
#include <array>

#include "InternalGlobals.hpp"

namespace dmg
{
    class Tilemap
    {
    public:
        Tilemap() = default;
        ~Tilemap() = default;

        [[nodiscard]] uint8_t Read(const uint16_t address) const
        {
            return m_data[address];
        }
        void Write(const uint16_t address, const uint8_t value)
        {
            m_data[address] = value;
        }
    private:
        std::array<uint8_t, globals::TILEMAP_AREA_SIZE> m_data {};
    };
}