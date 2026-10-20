#include "PPU/PixelFetcher.hpp"
#include "PPU/FIFO.hpp"
#include "PPU/PPU.hpp"

namespace dmg
{
    PixelFetcher::PixelFetcher(PPU& ppu, FIFO& fifo)
        : m_ppu(ppu)
        , m_fifo(fifo)
    {
    }

    void PixelFetcher::Step()
    {
        switch (m_state)
        {
        case State::GET_TILE_DATA:
            {
                if (m_progress == 0)
                {
                    const uint32_t fx = ((m_ppu.SCX.Read() / 8) + m_x) & 0x1F;
                    const uint32_t fy = (m_ppu.LY.Read() + m_ppu.SCY.Read()) & 255;
                    const uint32_t row = fy / 8;
                    const uint32_t line_in_tile = fy % 8;

                    const uint8_t tile_ref = m_ppu.VRAM[m_ppu.LCDC.GetBGTileMapArea() + fx + row * 32];
                    Tile tile = m_ppu.GetTileWindowBG(tile_ref);
                    m_tile_row = tile.GetRow(line_in_tile, m_ppu.VRAM);
                }
                if (++m_progress >= 2)
                {
                    m_state = State::GET_TILE_DATA_LOW;
                    m_progress = 0;
                }
            }
            break;
        case State::GET_TILE_DATA_LOW:
            if (m_progress == 0)
                m_tile_low = m_tile_row.first;
            if (++m_progress >= 2)
            {
                m_state = State::GET_TILE_DATA_HIGH;
                m_progress = 0;
            }
            break;
        case State::GET_TILE_DATA_HIGH:
            if (m_progress == 0)
                m_tile_high = m_tile_row.second;
            if (++m_progress >= 2)
            {
                m_state = State::SLEEP;
                m_progress = 0;
            }
            break;
        case State::SLEEP:
            if (++m_progress >= 2)
            {
                m_state = State::PUSH;
                m_progress = 0;
            }
            break;
        case State::PUSH:
            if (m_fifo.IsEmpty())
            {
                m_fifo.PushPixels(m_tile_low, m_tile_high);
                m_state = State::GET_TILE_DATA;
                m_progress = 0;

                m_x += 1;
                if (m_x > 31)
                {
                    m_x = 0;
                    m_y += 1;
                }
            }
            break;
        }
    }

    void PixelFetcher::Reset()
    {
        m_x = 0;
        m_y = 0;

        m_state = State::GET_TILE_DATA;
        m_progress = 0;
    }
}
