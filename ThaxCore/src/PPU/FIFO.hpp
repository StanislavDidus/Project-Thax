#pragma once
#include <array>
#include <cstdint>

#include "InternalGlobals.hpp"
#include "CPU/Registers/AByteRegister.hpp"
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

        explicit FIFO(PPU& ppu);
        ~FIFO() = default;

        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] bool IsDone() const;
        [[nodiscard]] bool IsScanlineDone() const;
        [[nodiscard]] int GetSize() const;

        void PushPixels(uint8_t low, uint8_t high);
        void PushObjectPixels(uint8_t low, uint8_t high, ObjectPalette palette);
        bool SendPixelToLCD(globals::LCD_BUFFER& lcd);
        void Reset();
        void MakeEmpty();
        void StartScanline();

        void SetDiscardPixels(int discard_pixels);
    private:
        [[nodiscard]] static uint8_t GetColorByID(const AByteRegister& reg, uint8_t id);
        [[nodiscard]] uint8_t GetColorFromPalette(const FIFOPixel& pixel) const;

        void CombineTwoFIFOs();

        std::array<FIFOPixel, 16> m_pixels {};
        std::array<FIFOPixel, 16> m_pixels_objects {};
        uint32_t m_address = 0;
        uint32_t m_size = 0;
        uint32_t m_scanline_progress = 0;

        PPU& m_ppu;

        bool m_fine_scroll = false;

        int m_discard_pixels = 0;
    };
}
