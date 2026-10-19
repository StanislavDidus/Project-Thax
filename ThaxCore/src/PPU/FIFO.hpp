#pragma once
#include <array>
#include <cstdint>

#include "InternalGlobals.hpp"

namespace dmg
{
    class FIFO
    {
    public:
        struct FIFOPixel
        {
            uint8_t m_color = 0;
            PixelSource m_source = PixelSource::BG;
        };

        FIFO() = default;
        ~FIFO() = default;

        [[nodiscard]] bool IsEmpty() const;
        void PushPixels(uint8_t low, uint8_t high);
    private:
        void AddPixel(uint8_t low, uint8_t high, uint8_t index);

        std::array<FIFOPixel, 8> m_pixels {};
    };
}
