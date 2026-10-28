#include "FIFO.hpp"

#include "Precompiled.hpp"

#include "PPU.hpp"
#include "Utils/Math.hpp"

namespace dmg
{
    FIFO::FIFO(PPU& ppu)
        : m_ppu(ppu)
    {
    }

    bool FIFO::IsEmpty() const
    {
        return m_size == 0;
    }

    bool FIFO::IsDone() const
    {
        return m_address >= 23040;
    }

    bool FIFO::IsScanlineDone() const
    {
        return m_scanline_progress >= 160;
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
            AddPixel(low, high, i);
        }
        m_size += (8 - start_point);
    }

    bool FIFO::SendPixelToLCD(globals::LCD_BUFFER& lcd)
    {
        if (!IsEmpty())
        {
            lcd[m_address++] = m_pixels[0].m_color;
            std::move(m_pixels.begin() + 1, m_pixels.end(), m_pixels.begin());
            m_size--;
            m_scanline_progress += 1;

            return true;
        }

        return false;
    }

    void FIFO::ResetBuffer()
    {
        std::fill_n(m_pixels.begin(), m_pixels.size(), FIFOPixel{});
        m_size = 0;
        m_scanline_progress = 0;
    }

    void FIFO::Reset()
    {
        m_address = 0;
        ResetBuffer();
    }

    void FIFO::MakeEmpty()
    {
        m_size = 0;
    }

    void FIFO::StartScanline()
    {
        m_size = 0;
        m_scanline_progress = 0;
        m_discard_pixels = 0;
        //m_fine_scroll = false;
    }

    void FIFO::DiscardPixels(const int discard_pixels)
    {
        m_discard_pixels += discard_pixels;
    }

    void FIFO::AddPixel(const uint8_t low, const uint8_t high, const uint8_t index)
    {
        m_pixels[index] = FIFOPixel{
            .m_color = static_cast<uint8_t>((getBit(high, (7 - index)) << 1) | getBit(low, (7 - index))),
            .m_source = PixelSource::BG
        };
    }
}
