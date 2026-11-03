#pragma once
#include <array>
#include <cstdint>
#include <span>

#include "InternalGlobals.hpp"

namespace dmg
{
    class Tile
    {
    public:
        using TileRow = std::pair<uint8_t, uint8_t>;

        Tile() = default;
        explicit Tile(const uint16_t address) : m_address(address) {}
        ~Tile() = default;

        [[nodiscard]] TileRow GetRow(const uint32_t row, const std::span<const uint8_t> memory) const
        {
            return TileRow{
                memory[m_address + row * 2],
                memory[m_address + row * 2 + 1]
            };
        }

    private:
        uint16_t m_address = 0;
    };
}
