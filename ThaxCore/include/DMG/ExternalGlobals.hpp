#pragma once

#include <array>
#include <cstdint>

namespace dmg::globals
{
    using LCD_BUFFER = std::array<uint8_t, 160 * 144>;
    using TILE_DATA_BUFFER = std::array<uint8_t, 384 * 64>;

    // ASSET TILE DATA VIEW
    inline constexpr uint32_t TILE_DATA_VIEW_SIZE = 378;
    inline constexpr uint32_t TILE_DATA_WIDTH = 16;
    inline constexpr uint32_t TILE_DATA_HEIGHT = 24;
    inline constexpr uint32_t TILE_DATA_VIEW_WIDTH = TILE_DATA_WIDTH * 8;
    inline constexpr uint32_t TILE_DATA_VIEW_HEIGHT = TILE_DATA_HEIGHT * 8;

    inline constexpr uint32_t LCD_WIDTH = 160;
    inline constexpr uint32_t LCD_HEIGHT = 144;
}
