#include "Precompiled.hpp"
#include "FIFO.hpp"
#include "PPU.hpp"
#include "Registers/PaletteRegister.hpp"

namespace dmg
{
    FIFO::FIFO(PPU& ppu)
        : m_ppu(ppu)
    {
    }

    bool FIFO::IsEmpty() const
    {
        return m_size <= 8;
    }

    bool FIFO::IsDone() const
    {
        return m_address == globals::LCD_WIDTH * globals::LCD_HEIGHT;
    }

    bool FIFO::IsScanlineDone() const
    {
        return m_scanline_progress == globals::LCD_WIDTH;
    }

    int FIFO::GetSize() const
    {
        return static_cast<int>(m_size);
    }

    void FIFO::PushPixels(const uint8_t low, const uint8_t high)
    {
        int start_point = 0;

        if (m_discard_pixels > 0)
        {
            start_point = m_discard_pixels;
            m_discard_pixels = 0;
        }

        for (int i = start_point; i < 8; ++i)
        {
            const auto color_id = static_cast<uint8_t>((getBit(high, i) << 1) | getBit(low, i));

            m_pixels[7 - i + m_size] = FIFOPixel{
                .m_color = color_id,
                .m_source = PixelSource::BG
            };
        }
        m_size += 8 - start_point;
    }

    void FIFO::PushObjectPixels(uint8_t low, uint8_t high, ObjectPalette palette)
    {
        int start_point = 0;

        if (m_discard_pixels > 0)
        {
            start_point = m_discard_pixels;
            m_discard_pixels = 0;
        }

        for (int i = start_point; i < 8; ++i)
        {
            const auto color_id = static_cast<uint8_t>((getBit(high, i) << 1) | getBit(low, i));

            m_pixels_objects[7 - i] = FIFOPixel{
                .m_color = color_id,
                .m_source = palette == ObjectPalette::OBP0 ? PixelSource::OBP0 : PixelSource::OBP1
            };
        }
        CombineTwoFIFOs();
    }

    bool FIFO::SendPixelToLCD(globals::LCD_BUFFER& lcd)
    {
        if (!IsEmpty())
        {
            const FIFOPixel pixel = m_pixels[0];
            uint8_t final_color = GetColorFromPalette(pixel);

            // Display Color 0 if BG/Window are disabled
            if (pixel.m_source == PixelSource::BG && !m_ppu.LCDC.IsBGWindowEnabled()) final_color = GetColorByID(m_ppu.BGP, 0);

            lcd[m_address++] = final_color;
            std::move(m_pixels.begin() + 1, m_pixels.end(), m_pixels.begin());
            m_size--;
            m_scanline_progress += 1;

            return true;
        }

        return false;
    }

    void FIFO::Reset()
    {
        MakeEmpty();
        m_address = 0;
        m_scanline_progress = 0;
        m_discard_pixels = 0;
    }

    void FIFO::MakeEmpty()
    {
        m_size = 0;
    }

    void FIFO::StartScanline()
    {
        MakeEmpty();
        m_scanline_progress = 0;
        m_discard_pixels = 0;
    }

    void FIFO::SetDiscardPixels(const int discard_pixels)
    {
        m_discard_pixels = discard_pixels;
    }

    uint8_t FIFO::GetColorByID(const AByteRegister& reg, uint8_t id)
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

    void FIFO::CombineTwoFIFOs()
    {
        for (int i = 0; i < 8; ++i)
        {
            FIFOPixel& p1 = m_pixels[i];
            const FIFOPixel& p2 = m_pixels_objects[i];

            if (p2.m_color == 0) continue;
            if (p1.m_source != PixelSource::BG) continue;
            p1 = p2;
        }

        std::ranges::fill(m_pixels_objects, FIFOPixel{});
    }
}
