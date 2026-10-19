#include "PPU.hpp"

#include <iostream>

#include "Miscellaneous/InterruptController.hpp"
#include "Utils/Math.hpp"

namespace dmg
{
    PPU::PPU(InterruptController& interrupt_controller)
        : m_interrupt_controller(interrupt_controller)
    {
        SetMode(PPUMode::MODE2);
    }

    void PPU::Update(const uint32_t cycles)
    {
        //if (!LCDC.IsLCDEnabled()) return;

        m_current_cycles += cycles;

        UpdateMode(m_mode);
    }

    const globals::LCD_BUFFER& PPU::getLCDBuffer() const
    {
        return m_lcd;
    }

    globals::TILE_DATA_BUFFER PPU::getTileDataBuffer()
    {
        globals::TILE_DATA_BUFFER tile_data_buffer {};

        for (int row = 0; row < 8; ++row)
        {
            for (int i = 0; i < 384; ++i)
            {
                Tile tile {static_cast<uint16_t>(i * 16)};

                const Tile::TileRow tile_row = tile.GetRow(row, VRAM);
                uint8_t low = tile_row.first;
                uint8_t high = tile_row.second;

                for (int j = 0; j < 8; ++j)
                {
                    uint8_t color = (getBit(high, (7 - j)) << 1) | getBit(low, (7 - j));
                    tile_data_buffer[i * 8 + j + row * 384 * 8] = color;

                    int x = (i % 16) * 8 + j;
                    int y = (i / 16) * 8 + row;
                    tile_data_buffer[y * 128 + x] = color;
                }
            }
        }

        return tile_data_buffer;
    }

    void PPU::WriteVRAM(const uint16_t address, const uint8_t value)
    {
        VRAM[address] = value;
    }

    void PPU::WriteOAM(const uint16_t address, const uint8_t value)
    {
        OAM[address] = value;
    }

    void PPU::WriteIO(const uint16_t address, const uint8_t value)
    {
        switch (address)
        {
        case 0x00: LCDC.Write(value);
        case 0x01: STAT.Write(value);
        case 0x02: SCY.Write(value);
        case 0x03: SCX.Write(value);
        case 0x04: LY.Write(value);
        case 0x05: LYC.Write(value);
        case 0x06: DMA.Write(value);
        case 0x07: BGP.Write(value);
        case 0x08: OBP0.Write(value);
        case 0x09: OBP1.Write(value);
        case 0x0A: WY.Write(value);
        case 0x0B: WX.Write(value);
        default: ;
        }
    }

    uint8_t PPU::ReadVRAM(const uint16_t address) const
    {
        return VRAM[address];
    }

    uint8_t PPU::ReadOAM(const uint16_t address) const
    {
        return OAM[address];
    }

    uint8_t PPU::ReadIO(uint16_t address) const
    {
        switch (address)
        {
        case 0x00: return LCDC.Read();
        case 0x01: return STAT.Read();
        case 0x02: return SCY.Read();
        case 0x03: return SCX.Read();
        case 0x04: return LY.Read();
        case 0x05: return LYC.Read();
        case 0x06: return DMA.Read();
        case 0x07: return BGP.Read();
        case 0x08: return OBP0.Read();
        case 0x09: return OBP1.Read();
        case 0x0A: return WY.Read();
        case 0x0B: return WX.Read();
        default: return 0xFF;
        }
    }

    void PPU::SetMode(const PPUMode mode)
    {
        if (m_mode == mode) return;

        if (m_mode != PPUMode::NONE)
            ExitMode(m_mode);

        m_mode = mode;

        EnterMode(m_mode);
    }

    void PPU::EnterMode(PPUMode mode)
    {
        switch (mode)
        {
        case PPUMode::MODE0:
            break;
        case PPUMode::MODE1:
            m_interrupt_controller.SetIF(InterruptController::VBLANK);
            break;
        case PPUMode::MODE2:
            break;
        case PPUMode::MODE3:
            fetcher_x = 0;
            fetcher_y = 0;
            break;
        }
    }

    void PPU::UpdateMode(const PPUMode mode)
    {
        switch (mode)
        {
        case PPUMode::MODE0:
            if (m_current_cycles >= globals::PPU_MODE_0_LENGTH)
            {
                SetMode(PPUMode::MODE1);
                m_current_cycles = 0;
            }
            break;
        case PPUMode::MODE1:
            if (m_current_cycles >= globals::PPU_MODE_1_LENGTH)
            {
                SetMode(PPUMode::MODE2);
                m_current_cycles = 0;
            }
            break;
        case PPUMode::MODE2:
            if (m_current_cycles >= globals::PPU_MODE_2_LENGTH)
            {
                SetMode(PPUMode::MODE3);
                m_current_cycles = 0;
            }
            break;
        case PPUMode::MODE3:
            {
                if (m_current_cycles >= globals::PPU_MODE_3_LENGTH)
                {
                    SetMode(PPUMode::MODE0);
                    m_current_cycles = 0;
                }
                /*const uint32_t fx = ((SCX.Read() / 8) + fetcher_x) & 0x1F;
                const uint32_t fy = (LY.Read() + SCY.Read()) & 255;
                const uint32_t row = fy / 8;
                const uint32_t line_in_tile = row % 8;

                const uint8_t tile_ref = VRAM[globals::TILEMAP_AREA_1.m_start + fx + row * 32];
                Tile tile = GetTileWindowBG(tile_ref);
                const Tile::TileRow tile_row = tile.GetRow(line_in_tile, VRAM);
                const uint8_t low = tile_row.first;
                const uint8_t high = tile_row.second;
                if (m_fifo.IsEmpty())
                    m_fifo.PushPixels(low, high);

                fetcher_x += 1;
                if (fetcher_x > 31)
                {
                    fetcher_x = 0;
                    fetcher_y += 1;
                }*/
            }
            break;
        }
    }

    void PPU::ExitMode(PPUMode mode)
    {
        switch (mode)
        {
        case PPUMode::MODE0:
            LY.Write(LY.Read() + 1);
            if (LY.Read() >= 154) LY.Write(0);
            break;
        case PPUMode::MODE1:
            break;
        case PPUMode::MODE2:
            break;
        case PPUMode::MODE3:
            break;
        }
    }

    Tile PPU::GetTileWindowBG(const uint32_t index) const
    {
        return Tile{VRAM[LCDC.GetBGWindowTileDataArea() + index * 16]};
    }
}
