#include "PPU.hpp"

#include <iostream>

#include "Miscellaneous/InterruptController.hpp"
#include "Miscellaneous/DMAController.hpp"
#include "Registers/PaletteRegister.hpp"
#include "Utils/Math.hpp"

namespace dmg
{
    PPU::PPU(InterruptController& interrupt_controller, DMAController& dma_controller)
        : m_interrupt_controller(interrupt_controller)
        , m_dma_controller(dma_controller)
    {
        m_objects.reserve(globals::MAX_OBJECTS);

        Reset();
    }

    void PPU::Update(const uint32_t cycles)
    {
        if (!LCDC.IsLCDEnabled()) return;

        for (int i = 0; i < cycles / 4; ++i)
        {
            UpdateMode(m_mode);

            if (m_dma_controller.IsActive())
                m_dma_controller.Update();

            if (STAT.Update(m_mode, LYC.Read() == LY.Read()))
            {
                m_interrupt_controller.SetIF(InterruptController::LCD);
            }
        }
    }

    const globals::LCD_BUFFER& PPU::GetLCDBuffer() const
    {
        return m_lcd;
    }

    globals::TILE_DATA_BUFFER PPU::GetTileDataBuffer() const
    {
        globals::TILE_DATA_BUFFER tile_data_buffer = std::make_unique<std::vector<uint8_t>>();
        tile_data_buffer->resize(globals::TILE_DATA_VIEW_WIDTH * globals::TILE_DATA_VIEW_HEIGHT);

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
                        (*tile_data_buffer)[py * globals::TILE_DATA_VIEW_WIDTH + px] = color;
                    }
                }
            }
        }

        return tile_data_buffer;
    }

    globals::TILEMAP_BUFFER PPU::GetBGTilemapBuffer() const
    {
        globals::TILEMAP_BUFFER tilemap_buffer = std::make_unique<std::vector<uint8_t>>();
        tilemap_buffer->resize(globals::TILEMAP_DATA_WIDTH * globals::TILEMAP_DATA_HEIGHT);

        for (int y = 0; y < globals::TILEMAP_HEIGHT; ++y)
        {
            for (int row = 0; row < 8; ++row)
            {
                for (int x = 0; x < globals::TILEMAP_WIDTH; ++x)
                {
                    uint32_t tile_ref = VRAM[LCDC.GetBGTileMapArea() + x + y * globals::TILEMAP_WIDTH];
                    Tile tile = GetTileWindowBG(tile_ref);

                    const Tile::TileRow tile_row = tile.GetRow(row, VRAM);
                    uint8_t low = tile_row.first;
                    uint8_t high = tile_row.second;

                    for (int j = 0; j < 8; ++j)
                    {
                        uint8_t color = GetColorByID(BGP, (getBit(high, (7 - j)) << 1) | getBit(low, (7 - j)));

                        int px = x * 8 + j;
                        int py = y * 8 + row;
                        (*tilemap_buffer)[py * globals::TILEMAP_DATA_WIDTH + px] = color;
                    }
                }
            }
        }

        return std::move(tilemap_buffer);
    }

    globals::TILEMAP_BUFFER PPU::GetWindowTilemapBuffer() const
    {
        globals::TILEMAP_BUFFER tilemap_buffer = std::make_unique<std::vector<uint8_t>>();
        tilemap_buffer->resize(globals::TILEMAP_DATA_WIDTH * globals::TILEMAP_DATA_HEIGHT);

        for (int y = 0; y < globals::TILEMAP_HEIGHT; ++y)
        {
            for (int row = 0; row < 8; ++row)
            {
                for (int x = 0; x < globals::TILEMAP_WIDTH; ++x)
                {
                    uint32_t tile_ref = VRAM[LCDC.GetWindowTileMapArea() + x + y * globals::TILEMAP_WIDTH];
                    Tile tile = GetTileWindowBG(tile_ref);

                    const Tile::TileRow tile_row = tile.GetRow(row, VRAM);
                    uint8_t low = tile_row.first;
                    uint8_t high = tile_row.second;

                    for (int j = 0; j < 8; ++j)
                    {
                        uint8_t color = GetColorByID(BGP, (getBit(high, (7 - j)) << 1) | getBit(low, (7 - j)));

                        int px = x * 8 + j;
                        int py = y * 8 + row;
                        (*tilemap_buffer)[py * globals::TILEMAP_DATA_WIDTH + px] = color;
                    }
                }
            }
        }

        return std::move(tilemap_buffer);
    }

    void PPU::Reset()
    {
        VRAM.fill(0x00);
        OAM.fill(0x00);

        m_y_condition = false;
        fetcher_x = 0;
        fetcher_y = 0;

        LCDC.Write(0x00);
        STAT.Write(0x00);
        SCY.Write(0x00);
        SCX.Write(0x00);
        LY.Write(0x00);
        LYC.Write(0x00);
        BGP.Write(0x00);
        OBP0.Write(0x00);
        OBP1.Write(0x00);
        WY.Write(0x00);
        WX.Write(0x00);

        m_lcd.fill(0);

        m_objects.clear();

        m_current_cycles = 0;

        m_pixel_fetcher.Reset();
        m_fifo.Reset();

        SetMode(PPUMode::OAM_SCAN);
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

    void PPU::EnterMode(const PPUMode mode)
    {
        switch (mode)
        {
        case PPUMode::HBLANK:
            break;
        case PPUMode::VBLANK:
            m_interrupt_controller.SetIF(InterruptController::VBLANK);
            m_fifo.Reset();
            m_y_condition = false;
            m_pixel_fetcher.Reset();
            break;
        case PPUMode::OAM_SCAN:
            FindObjectsOnScanline();
            break;
        case PPUMode::DRAWING_PIXELS:
            m_pixel_fetcher.StartScanline();
            m_fifo.StartScanline();

            if (LY.Read() == WY.Read())
                m_y_condition = true;

            break;
        case PPUMode::NONE:
            break;
        }
    }

    void PPU::UpdateMode(const PPUMode mode)
    {
        switch (mode)
        {
        case PPUMode::HBLANK:
            break;
        case PPUMode::VBLANK:
            LY.Write(144 + m_current_cycles / 456);
            break;
        case PPUMode::OAM_SCAN:
            break;
        case PPUMode::DRAWING_PIXELS:
            {
                for (int i = 0; i < 4; ++i)
                {
                    m_pixel_fetcher.Step();
                    if (m_fifo.SendPixelToLCD(m_lcd))
                    {
                        if (m_fifo.GetXCounter() == WX.Read() && m_y_condition && LCDC.IsWindowEnabled())
                        {
                            m_fifo.Clear();
                            m_pixel_fetcher.StartWindow();
                        }
                        if (LCDC.AreObjectsEnabled())
                        {
                            if (auto object = GetObject())
                            {
                                m_fetch_object = object;
                            }
                        }
                    }

                    if (m_fetch_object.has_value() && m_fifo.GetSize() >= 8)
                    {
                        m_pixel_fetcher.FetchObject(m_fetch_object.value());
                        m_fetch_object = std::nullopt;
                    }

                    if (m_fifo.IsScanlineDone())
                    {
                        m_current_cycles = 0;
                        SetMode(PPUMode::HBLANK);
                        break;
                    }
                }

            }
            break;
        case PPUMode::NONE:
            break;
        }

        ProgressMode();
    }

    void PPU::ExitMode(const PPUMode mode)
    {
        switch (mode)
        {
        case PPUMode::HBLANK:
            break;
        case PPUMode::VBLANK:
            LY.Write(0);
            break;
        case PPUMode::OAM_SCAN:
            break;
        case PPUMode::DRAWING_PIXELS:
            m_pixel_fetcher.EndScanline();
            break;
        case PPUMode::NONE:
            break;
        }
    }

    void PPU::ProgressMode()
    {
        m_current_cycles += 4;

        switch (m_mode)
        {
        case PPUMode::OAM_SCAN:
            if (m_current_cycles >= globals::OAM_SCAN_LENGTH)
            {
                m_current_cycles = 0;
                SetMode(PPUMode::DRAWING_PIXELS);
            }
            break;
        case PPUMode::DRAWING_PIXELS:
            break;
        case PPUMode::HBLANK:
            if (m_current_cycles >= globals::HBLANK_LENGTH)
            {
                LY.Write(LY.Read() + 1);

                if (LY.Read() >= 144)
                {
                    m_current_cycles = 0;
                    SetMode(PPUMode::VBLANK);
                }
                else
                {
                    m_current_cycles = 0;
                    SetMode(PPUMode::OAM_SCAN);
                }
            }
            break;
        case PPUMode::VBLANK:
            if (m_current_cycles >= globals::VBLANK_LENGTH)
            {
                m_current_cycles = 0;
                SetMode(PPUMode::OAM_SCAN);
            }
            break;
        default: ;
        }
    }

    void PPU::FindObjectsOnScanline()
    {
        m_objects.clear();

        // Find all objects that intersect the Y scanline line
        for (int i = 0; i < 40; ++i)
        {
            Object object {static_cast<uint16_t>(i * 4), OAM};
            const int object_y = static_cast<int>(object.GetYPosition() - globals::OBJECT_Y_OFFSET);

            const int object_height = LCDC.GetObjectSize() == ObjectSize::SIZE_8X8 ? 8 : 16;
            if (LY.Read() >= object_y && LY.Read() < object_y + object_height)
            {
                m_objects.push_back(object);

                if (m_objects.size() >= globals::MAX_OBJECTS_PER_SCANLINE) break;
            }
        }

        // Sort objects by their x position and their address
        std::sort(m_objects.begin(), m_objects.end());
    }

    std::optional<Object> PPU::GetObject()
    {
        for (auto& object : m_objects)
        {
            if (object.GetXPosition() - globals::OBJECT_X_OFFSET <= m_fifo.GetXCounter() - 7 && !object.IsFetched())
            {
                object.SetFetched();
                return object;
            }
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
