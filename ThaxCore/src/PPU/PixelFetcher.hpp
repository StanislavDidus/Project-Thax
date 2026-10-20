#pragma once
#include <array>
#include <cstdint>

#include "Tile.hpp"

namespace dmg
{
    class PPU;
    class FIFO;

    class PixelFetcher
    {
    public:
        PixelFetcher(PPU& ppu, FIFO& fifo);
        ~PixelFetcher() = default;

        void Step();
        void Reset();

    private:
        enum class State
        {
            GET_TILE_DATA,
            GET_TILE_DATA_LOW,
            GET_TILE_DATA_HIGH,
            SLEEP,
            PUSH,
        };

        State m_state = State::GET_TILE_DATA;
        uint32_t m_progress = 0;

        uint32_t m_x = 0;
        uint32_t m_y = 0;

        Tile::TileRow m_tile_row {};
        uint8_t m_tile_low {};
        uint8_t m_tile_high {};

        PPU& m_ppu;
        FIFO& m_fifo;
    };
}
