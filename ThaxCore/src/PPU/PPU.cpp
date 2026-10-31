#include "PPU.hpp"

#include <iostream>

#include "Miscellaneous/InterruptController.hpp"
#include "Miscellaneous/DMAController.hpp"
#include "Utils/Math.hpp"

namespace dmg
{
    PPU::PPU(InterruptController& interrupt_controller, DMAController& dma_controller)
        : m_interrupt_controller(interrupt_controller)
        , m_dma_controller(dma_controller)
    {
        m_objects.reserve(globals::MAX_OBJECTS);

        SetMode(PPUMode::MODE2);
    }

    void PPU::Update(const uint32_t cycles)
    {
        if (!LCDC.IsLCDEnabled()) return;

        m_current_cycles += cycles;

        for (int i = 0; i < cycles / 4; ++i)
        {
            UpdateMode(m_mode, 4);

            if (m_dma_controller.IsActive())
                m_dma_controller.Update();

            if (STAT.Update(m_mode, LYC.Read() == LY.Read()))
            {
                m_interrupt_controller.SetIF(InterruptController::LCD);
            }
        }
    }

    const globals::LCD_BUFFER& PPU::getLCDBuffer() const
    {
        return m_lcd;
    }

    globals::TILE_DATA_BUFFER PPU::getTileDataBuffer()
    {
        globals::TILE_DATA_BUFFER tile_data_buffer {};

        for (int y = 0; y < globals::TILE_DATA_HEIGHT; ++y)
        {
            for (int row = 0; row < 8; ++row)
            {
                for (int x = 0; x < globals::TILE_DATA_WIDTH; ++x)
                {
                    int i = x + y * 16;
                    Tile tile {static_cast<uint16_t>(i * 16)};

                    const Tile::TileRow tile_row = tile.GetRow(row, VRAM);
                    uint8_t low = tile_row.first;
                    uint8_t high = tile_row.second;

                    for (int j = 0; j < 8; ++j)
                    {
                        uint8_t color = (getBit(high, (7 - j)) << 1) | getBit(low, (7 - j));

                        int px = x * 8 + j;
                        int py = y * 8 + row;
                        tile_data_buffer[py * globals::TILE_DATA_VIEW_WIDTH + px] = color;
                    }
                }
            }
        }

        return tile_data_buffer;
    }

    void PPU::WriteVRAM(const uint16_t address, const uint8_t value)
    {
        //if (m_mode == PPUMode::MODE3) return;

        VRAM[address] = value;
    }

    void PPU::WriteOAM(const uint16_t address, const uint8_t value)
    {
        //if (m_mode == PPUMode::MODE3) return;
        //if (m_mode == PPUMode::MODE2) return;

        OAM[address] = value;
    }

    void PPU::WriteIO(const uint16_t address, const uint8_t value)
    {
        switch (address)
        {
        case 0x00: LCDC.Write(value); break;
        case 0x01: STAT.Write(value); break;
        case 0x02: SCY.Write(value); break;
        case 0x03: SCX.Write(value); break;
        case 0x04: break;
        case 0x05: LYC.Write(value); break;
        case 0x06: m_dma_controller.Write(value); break;
        case 0x07: BGP.Write(value); break;
        case 0x08: OBP0.Write(value); break;
        case 0x09: OBP1.Write(value); break;
        case 0x0A: WY.Write(value); break;
        case 0x0B: WX.Write(value); break;
        default: break;
        }
    }

    uint8_t PPU::ReadVRAM(const uint16_t address) const
    {
        //if (m_mode == PPUMode::MODE3) return 0xFF;

        return VRAM[address];
    }

    uint8_t PPU::ReadOAM(const uint16_t address) const
    {
        //if (m_mode == PPUMode::MODE3) return 0xFF;
        //if (m_mode == PPUMode::MODE2) return 0xFF;

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
        case 0x06: return m_dma_controller.Read();
        case 0x07: return BGP.Read();
        case 0x08: return OBP0.Read();
        case 0x09: return OBP1.Read();
        case 0x0A: return WY.Read();
        case 0x0B: return WX.Read();
        default: return 0xFF;
        }
    }

    PPUInfo PPU::GetPPUInfo() const
    {
        return PPUInfo
        {
            .LCDC = LCDC.Read(),
            .STAT = STAT.Read(),
            .SCY = SCY.Read(),
            .SCX = SCX.Read(),
            .LY = LY.Read(),
            .LYC = LYC.Read(),
            .WX = WX.Read(),
            .WY = WY.Read(),
            .DMA = m_dma_controller.Read(),
            .BGP = BGP.Read(),
            .OBP0 = OBP0.Read(),
            .OBP1 = OBP1.Read(),
            .ppu_mode = m_mode,
            .pixel_fetcher_state = m_pixel_fetcher.GetState(),
            .fifo_pixels = m_fifo.GetSize()
        };
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
            m_fifo.Reset();
            m_y_condition = false;
            m_pixel_fetcher.Reset();
            break;
        case PPUMode::MODE2:
            FindObjectsOnScanline();
            break;
        case PPUMode::MODE3:
            m_pixel_fetcher.StartScanline();
            m_fifo.StartScanline();

            if (LY.Read() == WY.Read())
                m_y_condition = true;

            m_x_counter = 7;

            //int discard_pixels = SCX.Read() % 8;
            m_fifo.SetDiscardPixels(SCX.Read() % 8);

            break;
        }
    }

    void PPU::UpdateMode(const PPUMode mode, uint32_t cycles)
    {
        switch (mode)
        {
        case PPUMode::MODE0:
            if (m_current_cycles >= globals::PPU_MODE_0_LENGTH)
            {
                LY.Write(LY.Read() + 1);

                if (LY.Read() >= 144)
                {
                    SetMode(PPUMode::MODE1);
                    m_current_cycles = 0;
                }
                else
                {
                    SetMode(PPUMode::MODE2);
                    m_current_cycles = 0;
                }
            }
            break;
        case PPUMode::MODE1:
            LY.Write(144 + m_current_cycles / 456);

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
                if (m_fifo.IsDone() || m_fifo.IsScanlineDone())
                {
                    SetMode(PPUMode::MODE0);
                    m_current_cycles = 0;
                }
                else
                {
                    for (int i = 0; i < cycles; ++i)
                    {
                        m_pixel_fetcher.Step();
                        if (m_fifo.SendPixelToLCD(m_lcd))
                        {
                            m_x_counter += 1;

                            if (m_x_counter == WX.Read() && m_y_condition && LCDC.IsWindowEnabled())
                            {
                                m_fifo.MakeEmpty();
                                m_pixel_fetcher.StartWindow();
                            }

                            if (auto object = GetObject())
                            {
                                m_pixel_fetcher.FetchObject(object.value());
                            }
                        }

                        if (m_fifo.IsDone() || m_fifo.IsScanlineDone())
                        {
                            SetMode(PPUMode::MODE0);
                            m_current_cycles = 0;
                            break;
                        }
                    }
                }

            }
            break;
        }
    }

    void PPU::ExitMode(PPUMode mode)
    {
        switch (mode)
        {
        case PPUMode::MODE0:
            break;
        case PPUMode::MODE1:
            LY.Write(0);
            break;
        case PPUMode::MODE2:
            break;
        case PPUMode::MODE3:
            m_pixel_fetcher.EndScanline();
            break;
        }
    }

    void PPU::FindObjectsOnScanline()
    {
        m_objects.clear();

        // Find all objects that intersect the Y scanline line
        for (int i = 0; i < 40; ++i)
        {
            Object object {static_cast<uint16_t>(i * 4), OAM};
            const uint8_t object_y = object.GetYPosition() - 16;

            if (LY.Read() >= object_y && LY.Read() < object_y + globals::TILE_HEIGHT)
            {
                m_objects.push_back(object);
            }
        }

        // Sort objects by their x position and their address
        std::sort(m_objects.begin(), m_objects.end());

        // Pick first 10 objects
        if (m_objects.size() > globals::MAX_OBJECTS_PER_SCANLINE)
            m_objects.erase(m_objects.begin() + globals::MAX_OBJECTS_PER_SCANLINE, m_objects.end());
    }

    std::optional<Object> PPU::GetObject() const
    {
        for (const auto& object : m_objects)
        {
            if (object.GetXPosition() - 8 == m_x_counter)
                return object;
        }

        return std::nullopt;
    }

    Tile PPU::GetTileWindowBG(const uint32_t index) const
    {
        if (getBit(LCDC.Read(), 4))
        {
            return Tile{static_cast<uint16_t>(globals::TILE_BLOCK_0.m_start + index * 16)};
        }
        const int offset = static_cast<int8_t>(index) * 16;
        return Tile{static_cast<uint16_t>(globals::TILE_BLOCK_2.m_start + offset)};
    }

    Tile PPU::GetObjectTile(const uint32_t index) const
    {
        return Tile{static_cast<uint16_t>(globals::TILE_BLOCK_0.m_start + index * 16)};
    }
}
