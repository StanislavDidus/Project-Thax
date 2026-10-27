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
                    uint32_t fx = ((m_ppu.SCX.Read() / 8) + m_x) & 0x1F;
                    uint32_t fy = (m_ppu.LY.Read() + m_ppu.SCY.Read()) & 255;

                    uint16_t tile_map_area = m_inside_window ? m_ppu.LCDC.GetWindowTileMapArea() : m_ppu.LCDC.GetBGTileMapArea();

                    if (m_inside_window)
                    {
                        fx = m_x_window;
                        fy = m_y_window;
                    }

                    const uint32_t row = fy / 8;
                    const uint32_t line_in_tile = fy % 8;

                    m_tile_ref = m_ppu.VRAM[tile_map_area + fx + row * 32];
                    m_tile_row = line_in_tile;
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
            {
                const Tile tile = m_ppu.GetTileWindowBG(m_tile_ref);
                m_tile_low = tile.GetRow(m_tile_row, m_ppu.VRAM).first;
            }
            if (++m_progress >= 2)
            {
                m_state = State::GET_TILE_DATA_HIGH;
                m_progress = 0;
            }
            break;
        case State::GET_TILE_DATA_HIGH:
            if (m_progress == 0)
            {
                const Tile tile = m_ppu.GetTileWindowBG(m_tile_ref);
                m_tile_high = tile.GetRow(m_tile_row, m_ppu.VRAM).second;
            }
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

                if (m_inside_window)
                {
                    m_x_window += 1;
                }
            }
            break;
        }
    }

    void PixelFetcher::StartScanline()
    {
        m_x = 0;
        m_x_window = 0;

        m_state = State::GET_TILE_DATA;
        m_progress = 0;
    }

    void PixelFetcher::EndScanline()
    {
        if (m_inside_window)
        {
            m_y_window += 1;
            if (m_y_window > 18) m_y_window = 0;
        }

        m_inside_window = false;
    }

    void PixelFetcher::SetWindow(bool value)
    {
        m_inside_window = value;
    }
}
