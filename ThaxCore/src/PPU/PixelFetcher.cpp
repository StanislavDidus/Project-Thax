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
            if (m_progress == 0)
            {
                if (m_current_fetch_type == FetchType::TILE)
                    GetTileData();
                else if (m_current_fetch_type == FetchType::OBJECT)
                    GetObjectTileData();
            }
            AdvanceState();
            break;
        case PixelFetcherState::GET_TILE_DATA_LOW:
            if (m_progress == 0)
            {
                if (m_current_fetch_type == FetchType::TILE)
                    GetTileDataLow();
                else if (m_current_fetch_type == FetchType::OBJECT)
                    GetObjectTileDataLow();
            }
            AdvanceState();
            break;
        case PixelFetcherState::GET_TILE_DATA_HIGH:
            if (m_progress == 0)
            {
                if (m_current_fetch_type == FetchType::TILE)
                    GetTileDataHigh();
                else if (m_current_fetch_type == FetchType::OBJECT)
                    GetObjectTileDataHigh();
            }
            AdvanceState();
            break;
        case PixelFetcherState::PUSH:
            if (m_current_fetch_type == FetchType::TILE)
                PushTile();
            else if (m_current_fetch_type == FetchType::OBJECT)
                PushObject();
            break;
        }
    }

    void PixelFetcher::StartScanline()
    {
        m_x = 0;
        m_x_window = 0;

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
        m_inside_window = true;
        StartScanline();
    }

    void PixelFetcher::Reset()
    {
        m_x = 0;
        m_x_window = 0;

        m_y_window = 0;

        RestartState();
    }

    PixelFetcherState PixelFetcher::GetState() const
    {
        return m_state;
    }

    void PixelFetcher::GetTileData()
    {
        m_tile_ref = m_ppu.VRAM[m_tile_fetch.m_address];
        m_tile_row = m_tile_fetch.m_tile_row;
    }

    void PixelFetcher::GetObjectTileData()
    {
        m_tile_ref = m_object_fetch.m_address;
        m_tile_row = m_object_fetch.m_tile_row;
    }

    void PixelFetcher::GetTileDataLow()
    {
        const Tile tile = m_ppu.GetTileWindowBG(m_tile_ref);
        m_tile_low = tile.GetRow(m_tile_row, m_ppu.VRAM).first;
    }

    void PixelFetcher::GetObjectTileDataLow()
    {
        const Tile tile = m_ppu.GetObjectTile(m_tile_ref);
        m_tile_low = tile.GetRow(m_tile_row, m_ppu.VRAM).first;
    }

    void PixelFetcher::GetTileDataHigh()
    {
        const Tile tile = m_ppu.GetTileWindowBG(m_tile_ref);
        m_tile_high = tile.GetRow(m_tile_row, m_ppu.VRAM).second;

    }

    void PixelFetcher::GetObjectTileDataHigh()
    {
        const Tile tile = m_ppu.GetObjectTile(m_tile_ref);
        m_tile_high = tile.GetRow(m_tile_row, m_ppu.VRAM).second;
    }

    void PixelFetcher::PushTile()
    {
        if (m_fifo.CanPush())
        {
            m_fifo.PushPixels(m_tile_low, m_tile_high);

            m_x += 1;

            if (m_inside_window)
            {
                m_x_window += 1;
            }

            FetchTile();
        }
    }

    void PixelFetcher::PushObject()
    {
        if (m_fifo.GetSize() >= 8)
        {
            m_fifo.PushObjectPixels(m_tile_low, m_tile_high,  m_object_fetch.m_object);
            m_state = PixelFetcherState::GET_TILE_DATA;
            m_progress = 0;

            if (const auto object = m_ppu.GetObject())
            {
                m_ppu.m_pixel_fetcher.FetchObject(object.value());
            }
            else
            {
                m_current_fetch_type = FetchType::TILE;
                m_ppu.m_fifo.SetStop(false);
            }
        }
    }

    void PixelFetcher::AdvanceState()
    {
        if (++m_progress >= 2)
        {
            switch (m_state)
            {
            case PixelFetcherState::GET_TILE_DATA: m_state = PixelFetcherState::GET_TILE_DATA_LOW; break;
            case PixelFetcherState::GET_TILE_DATA_LOW: m_state = PixelFetcherState::GET_TILE_DATA_HIGH; break;
            case PixelFetcherState::GET_TILE_DATA_HIGH: m_state = PixelFetcherState::PUSH; break;
            default: ;
            }
            m_progress = 0;
        }
    }

    void PixelFetcher::RestartState()
    {
        m_progress = 0;
        m_state = PixelFetcherState::GET_TILE_DATA;
    }

    void PixelFetcher::FetchTile()
    {
        m_current_fetch_type = FetchType::TILE;

        const uint32_t pixel_x = m_ppu.SCX.Read() + m_x * 8;

        const uint16_t tile_map_area = m_inside_window ? m_ppu.LCDC.GetWindowTileMapArea() : m_ppu.LCDC.GetBGTileMapArea();

        uint32_t fx, fy;
        if (m_inside_window)
        {
            fx = m_x_window;
            fy = m_y_window;
        }
        else
        {
            fx = (pixel_x / 8) & 0x1F;
            fy = (m_ppu.LY.Read() + m_ppu.SCY.Read()) & 255;
        }

        const uint32_t row = fy / 8;
        const uint32_t line_in_tile = fy % 8;

        m_tile_fetch = TileFetchData{
            .m_address = static_cast<uint16_t>(tile_map_area + fx + row * 32),
            .m_tile_row = static_cast<uint8_t>(line_in_tile)
        };

        RestartState();
    }

    void PixelFetcher::FetchObject(const Object& object)
    {
        m_current_fetch_type = FetchType::OBJECT;

        m_fifo.SetStop(true);

        const uint32_t fy = m_ppu.LY.Read() & 255;
        uint32_t line_in_tile = fy - (object.GetYPosition() - globals::OBJECT_Y_OFFSET);
        uint16_t address = object.GetTileIndex();

        const bool is_tall = m_ppu.LCDC.GetObjectSize() == ObjectSize::SIZE_8X16;
        const uint8_t height = is_tall ? 16 : 8;

        if (object.IsYFlipped())
            line_in_tile = height - 1 - line_in_tile;

        if (is_tall)
        {
            const bool is_bottom_tile = line_in_tile > 7;
            address = (address & 0xFE) | static_cast<int>(is_bottom_tile);
            line_in_tile &= 0x07;
        }

        m_object_fetch = ObjectFetchData{
            .m_address = address,
            .m_tile_row = static_cast<uint8_t>(line_in_tile),
            .m_object = object
        };

        RestartState();
    }
}
