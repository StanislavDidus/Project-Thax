#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace dmg
{
    enum class PPUMode : uint8_t
    {
        NONE,
        MODE0,
        MODE1,
        MODE2,
        MODE3,
    };

    enum class PixelFetcherState
    {
        GET_TILE_DATA,
        GET_TILE_DATA_LOW,
        GET_TILE_DATA_HIGH,
        PUSH,
    };
}
namespace dmg::globals
{
    inline constexpr int TILE_SIZE = 8;

    inline constexpr uint32_t TILE_DATA_VIEW_SIZE = 378;
    inline constexpr uint32_t TILE_DATA_WIDTH = 16;
    inline constexpr uint32_t TILE_DATA_HEIGHT = 24;
    inline constexpr uint32_t TILE_DATA_VIEW_WIDTH = TILE_DATA_WIDTH * TILE_SIZE;
    inline constexpr uint32_t TILE_DATA_VIEW_HEIGHT = TILE_DATA_HEIGHT * TILE_SIZE;

    inline constexpr int TILEMAP_WIDTH = 32;
    inline constexpr int TILEMAP_HEIGHT = 32;
    inline constexpr int TILEMAP_DATA_WIDTH = TILEMAP_WIDTH * TILE_SIZE;
    inline constexpr int TILEMAP_DATA_HEIGHT = TILEMAP_HEIGHT * TILE_SIZE;

    inline constexpr uint32_t LCD_WIDTH = 160;
    inline constexpr uint32_t LCD_HEIGHT = 144;

    using LCD_BUFFER = std::array<uint8_t, LCD_WIDTH * LCD_HEIGHT>;
    using TILE_DATA_BUFFER = std::array<uint8_t, TILE_DATA_VIEW_WIDTH * TILE_DATA_VIEW_HEIGHT>;
    using TILEMAP_BUFFER = std::unique_ptr<std::vector<uint8_t>>;
}
