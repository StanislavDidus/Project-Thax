#pragma once
#include <array>
#include <cstdint>

#include "InternalGlobals.hpp"
#include "DMG/ExternalGlobals.hpp"

namespace dmg
{
    class PPU;

    class FIFO
    {
    public:
        struct FIFOPixel
        {
            uint8_t m_color = 0;
            PixelSource m_source = PixelSource::BG;
        };

        FIFO(PPU& ppu);
        ~FIFO() = default;

        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] bool IsDone() const;
        [[nodiscard]] bool IsScanlineDone() const;

        void PushPixels(uint8_t low, uint8_t high);
        void SendPixelToLCD(globals::LCD_BUFFER& lcd);
        void ResetBuffer();
        void Reset();
        void MakeEmpty();
        void StartScanline();
    private:
        void AddPixel(uint8_t low, uint8_t high, uint8_t index);

        std::array<FIFOPixel, 16> m_pixels {};
        uint32_t m_address = 0;
        uint32_t m_size = 0;
        uint32_t m_scanline_progress = 0;

        PPU& m_ppu;

        bool m_fine_scroll = false;
    };
}
