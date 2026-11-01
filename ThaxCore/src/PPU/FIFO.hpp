#pragma once
#include <array>
#include <cstdint>

#include "InternalGlobals.hpp"
#include "CPU/Registers/AByteRegister.hpp"
#include "DMG/ExternalGlobals.hpp"

namespace dmg
{
    class Object;
    class PPU;

    class FIFO
    {
    public:
        explicit FIFO(PPU& ppu);
        ~FIFO() = default;

        [[nodiscard]] bool CanPush() const;
        [[nodiscard]] bool IsScanlineDone() const;
        [[nodiscard]] int GetSize() const;
        [[nodiscard]] int GetXCounter() const;

        void PushPixels(uint8_t low, uint8_t high);
        void PushObjectPixels(uint8_t low, uint8_t high, const Object& object);
        bool SendPixelToLCD(globals::LCD_BUFFER& lcd); // Returns true on success

        void Reset();
        void Clear();
        void StartScanline();

        void SetStop(bool value);
    private:
        struct FIFOPixel
        {
            uint8_t m_color = 0;
            PixelSource m_source = PixelSource::BG;
        };

        [[nodiscard]] static uint8_t GetColorByID(const AByteRegister& reg, uint8_t id);
        [[nodiscard]] uint8_t GetColorFromPalette(const FIFOPixel& pixel) const;

        std::array<FIFOPixel, 16> m_pixels {};
        uint32_t m_address = 0;
        uint32_t m_size = 0;
        uint32_t m_scanline_progress = 0;
        int m_discard_pixels = 0;
        bool m_stopped = false;

        PPU& m_ppu;
    };
}
