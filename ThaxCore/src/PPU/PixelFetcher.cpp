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
        case PixelFetcherState::GET_TILE_DATA:
            {
                if (m_progress == 0)
                {
                    if (m_current_fetch.m_type == FetchType::TILE)
                        m_tile_ref = m_ppu.VRAM[m_current_fetch.m_address];
                    else if (m_current_fetch.m_type == FetchType::OBJECT)
                        m_tile_ref = m_current_fetch.m_address;
                    m_tile_row = m_current_fetch.m_tile_row;
                }
                if (++m_progress >= 2)
                {
                    m_state = PixelFetcherState::GET_TILE_DATA_LOW;
                    m_progress = 0;
                }
            }
            break;
        case PixelFetcherState::GET_TILE_DATA_LOW:
            if (m_progress == 0)
            {
                Tile tile {};
                if (m_current_fetch.m_type == FetchType::TILE)
                    tile = m_ppu.GetTileWindowBG(m_tile_ref);
                else if (m_current_fetch.m_type == FetchType::OBJECT)
                    tile = m_ppu.GetObjectTile(m_tile_ref);
                m_tile_low = tile.GetRow(m_tile_row, m_ppu.VRAM).first;
            }
            if (++m_progress >= 2)
            {
                m_state = PixelFetcherState::GET_TILE_DATA_HIGH;
                m_progress = 0;
            }
            break;
        case PixelFetcherState::GET_TILE_DATA_HIGH:
            if (m_progress == 0)
            {
                Tile tile {};
                if (m_current_fetch.m_type == FetchType::TILE)
                    tile = m_ppu.GetTileWindowBG(m_tile_ref);
                else if (m_current_fetch.m_type == FetchType::OBJECT)
                    tile = m_ppu.GetObjectTile(m_tile_ref);
                m_tile_high = tile.GetRow(m_tile_row, m_ppu.VRAM).second;
            }
            if (++m_progress >= 2)
            {
                m_state = PixelFetcherState::PUSH;
                m_progress = 0;
            }
            break;
        case PixelFetcherState::PUSH:
            if (m_current_fetch.m_type == FetchType::OBJECT)
            {
                m_fifo.PushObjectPixels(m_tile_low, m_tile_high,  m_current_fetch.m_object_palette);
                m_state = PixelFetcherState::GET_TILE_DATA;
                m_progress = 0;
                    FetchTile();
            }
            else if (m_current_fetch.m_type == FetchType::TILE)
            {
                if (m_fifo.IsEmpty())
                {
                    m_fifo.PushPixels(m_tile_low, m_tile_high);
                    m_state = PixelFetcherState::GET_TILE_DATA;
                    m_progress = 0;

                    m_x += 1;

                    if (m_inside_window)
                    {
                        m_x_window += 1;
                    }

                    FetchTile();
                }
            }
        }
    }

    void PixelFetcher::StartScanline()
    {
        m_x = 0;
        m_x_window = 0;

        m_state = PixelFetcherState::GET_TILE_DATA;
        m_progress = 0;

        FetchTile();
    }

    void PixelFetcher::EndScanline()
    {
        if (m_inside_window)
        {
            m_y_window += 1;
        }

        m_inside_window = false;
    }

    void PixelFetcher::StartWindow()
    {
        StartScanline();
        m_inside_window = true;
        FetchTile();
    }

    void PixelFetcher::Reset()
    {
        m_y_window = 0;
    }

    PixelFetcherState PixelFetcher::GetState() const
    {
        return m_state;
    }

    void PixelFetcher::FetchTile()
    {
        const uint32_t pixel_x = m_ppu.SCX.Read() + m_x * 8;
        uint32_t fx = (pixel_x / 8) & 0x1F;
        uint32_t fy = (m_ppu.LY.Read() + m_ppu.SCY.Read()) & 255;

        const uint16_t tile_map_area = m_inside_window ? m_ppu.LCDC.GetWindowTileMapArea() : m_ppu.LCDC.GetBGTileMapArea();

        if (m_inside_window)
        {
            fx = m_x_window;
            fy = m_y_window;
        }

        const uint32_t row = fy / 8;
        const uint32_t line_in_tile = fy % 8;

        m_current_fetch = FetchData{
            .m_type = FetchType::TILE,
            .m_address = static_cast<uint16_t>(tile_map_area + fx + row * 32),
            .m_tile_row = static_cast<uint8_t>(line_in_tile)
        };
    }

    void PixelFetcher::FetchObject(const Object& object)
    {
        m_last_fetch = m_current_fetch;

        const uint32_t fy = m_ppu.LY.Read() & 255;
        const uint32_t line_in_tile = fy - (object.GetYPosition() - 16);

        m_current_fetch = FetchData{
            .m_type = FetchType::OBJECT,
            .m_object_palette = object.GetPalette(),
            .m_address = static_cast<uint16_t>(object.GetTileIndex()),
            .m_tile_row = static_cast<uint8_t>(line_in_tile)
        };

        m_state = PixelFetcherState::GET_TILE_DATA;
        m_progress = 0;
    }
}
