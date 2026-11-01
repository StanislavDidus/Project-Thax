#include "Precompiled.hpp"
#include "FIFO.hpp"
#include "PPU.hpp"
#include "Object.hpp"
#include "Registers/PaletteRegister.hpp"

namespace dmg
{
    FIFO::FIFO(PPU& ppu)
        : m_ppu(ppu)
    {
    }

    bool FIFO::CanPush() const
    {
        return m_size <= 8;
    }

    bool FIFO::IsScanlineDone() const
    {
        return m_scanline_progress >= globals::LCD_WIDTH + globals::LX_START;
    }

    int FIFO::GetSize() const
    {
        return static_cast<int>(m_size);
    }

    int FIFO::GetXCounter() const
    {
        return static_cast<int>(m_scanline_progress);
    }

    void FIFO::PushPixels(const uint8_t low, const uint8_t high)
    {
        for (int i = 0; i < 8; ++i)
        {
            const auto color_id = static_cast<uint8_t>((getBit(high, i) << 1) | getBit(low, i));

            m_pixels[7 - i + m_size] = FIFOPixel{
                .m_color = color_id,
                .m_source = PixelSource::BG
            };
        }
        m_size += 8;
    }

    void FIFO::PushObjectPixels(const uint8_t low, const uint8_t high, const Object& object)
    {
        for (int i = 0; i < 8; ++i)
        {
            const int bit = object.IsXFlipped() ? i : 7 - i;
            const auto color_id = static_cast<uint8_t>((getBit(high, bit) << 1) | getBit(low, bit));

            const FIFOPixel p2 {
                .m_color = color_id,
                .m_source = object.GetPalette() == ObjectPalette::OBP0 ? PixelSource::OBP0 : PixelSource::OBP1
            };

            FIFOPixel& p1 = m_pixels[i];

            if (object.GetPriority() && p1.m_source == PixelSource::BG && p1.m_color != 0) continue;
            if (p2.m_color == 0) continue;
            if (p1.m_source != PixelSource::BG) continue;
            p1 = p2;
        }
    }

    bool FIFO::SendPixelToLCD(globals::LCD_BUFFER& lcd)
    {
        if (!CanPush() && !m_stopped)
        {
            if (m_discard_pixels > 0)
            {
                std::move(m_pixels.begin() + m_discard_pixels, m_pixels.end(), m_pixels.begin());
                m_size -= m_discard_pixels;
                m_discard_pixels = 0;
            }

            const FIFOPixel pixel = m_pixels[0];
            uint8_t final_color = GetColorFromPalette(pixel);

            // Display Color 0 if BG/Window are disabled
            if (pixel.m_source == PixelSource::BG && !m_ppu.LCDC.IsBGWindowEnabled()) final_color = GetColorByID(m_ppu.BGP, 0);

            lcd[m_address++] = final_color;
            std::move(m_pixels.begin() + 1, m_pixels.end(), m_pixels.begin());
            m_size--;
            m_scanline_progress++;

            return true;
        }

        return false;
    }

    void FIFO::Reset()
    {
        StartScanline();
        m_address = 0;
    }

    void FIFO::Clear()
    {
        m_size = 0;
    }

    void FIFO::StartScanline()
    {
        Clear();
        m_scanline_progress = globals::LX_START;
        m_discard_pixels = m_ppu.SCX.Read() % globals::TILE_WIDTH;
    }

    void FIFO::SetStop(const bool value)
    {
        m_stopped = value;
    }

    uint8_t FIFO::GetColorByID(const AByteRegister& reg, const uint8_t id)
    {
        switch (id & 0b11)
        {
        case 0:
            return reg.Read() & 0b11;
            break;
        case 1:
            return (reg.Read() & 0b1100) >> 2;
            break;
        case 2:
            return (reg.Read() & 0b110000) >> 4;
            break;
        case 3:
            return (reg.Read() & 0b11000000) >> 6;
            break;
        default:
            DMG_ERROR("Program should never reach this point. PaletteRegister::GetColorByID()");
            return -1;
        }
    }

    uint8_t FIFO::GetColorFromPalette(const FIFOPixel& pixel) const
    {
        switch (pixel.m_source)
        {
        case PixelSource::BG: return GetColorByID(m_ppu.BGP, pixel.m_color);
        case PixelSource::OBP0: return GetColorByID(m_ppu.OBP0, pixel.m_color);
        case PixelSource::OBP1: return GetColorByID(m_ppu.OBP1, pixel.m_color);
        default: return 0;
        }
    }
}
