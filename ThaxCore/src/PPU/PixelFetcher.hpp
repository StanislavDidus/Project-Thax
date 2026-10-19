#pragma once
#include <array>
#include <cstdint>

namespace dmg
{
    class PixelFetcher
    {
    public:
        PixelFetcher() = default;
        ~PixelFetcher() = default;

        

    private:
        std::array<uint8_t, 16> m_pixels;
    };
}
