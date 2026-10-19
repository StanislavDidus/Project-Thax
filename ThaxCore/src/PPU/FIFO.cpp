#include "FIFO.hpp"

#include "Utils/Math.hpp"

namespace dmg
{
    bool FIFO::IsEmpty() const
    {
        return m_pixels.empty();
    }

    void FIFO::PushPixels(const uint8_t low, const uint8_t high)
    {
        for (int i = 0; i < 8; ++i)
        {
            AddPixel(low, high, 0);
        }
    }

    void FIFO::AddPixel(const uint8_t low, const uint8_t high, const uint8_t index)
    {
        m_pixels[index] = FIFOPixel{
            .m_color = (getBit(high, (7 - index)) << 1) || getBit(low, (7 - index)),
            .m_source = PixelSource::BG
        };
    }
}
