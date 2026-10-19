#pragma once

#include <array>
#include <cstdint>

namespace dmg::globals
{
    using LCD_BUFFER = std::array<uint8_t, 160 * 144>;
    using TILE_DATA_BUFFER = std::array<uint8_t, 384 * 64>;
}
