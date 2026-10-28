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
        void StartScanline();
        void EndScanline();
        void Reset();

        void SetWindow(bool value);

    private:
        enum class State
        {
            GET_TILE_DATA,
            GET_TILE_DATA_LOW,
            GET_TILE_DATA_HIGH,
            PUSH,
        };

        State m_state = State::GET_TILE_DATA;
        uint32_t m_progress = 0;

        uint32_t m_x = 0;

        uint16_t m_tile_ref = 0;
        uint8_t m_tile_row = 0;
        uint8_t m_tile_low = 0;
        uint8_t m_tile_high = 0;

        // Window
        bool m_inside_window = false;
        uint16_t m_x_window = 0;
        uint16_t m_y_window = 0;

        PPU& m_ppu;
        FIFO& m_fifo;
    };
}
