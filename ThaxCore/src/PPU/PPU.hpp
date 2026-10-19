#pragma once
#include <array>
#include <cstdint>

#include "FIFO.hpp"
#include "PixelFetcher.hpp"
#include "Tile.hpp"
#include "Object.hpp"
#include "CPU/Registers/ByteRegister.hpp"
#include "Tilemap.hpp"
#include "DMG/ExternalGlobals.hpp"
#include "Registers/LCDCRegister.hpp"
#include "Registers/STATRegister.hpp"

namespace dmg
{
    class InterruptController;

    class PPU
    {
    public:
        explicit PPU(InterruptController& interrupt_controller);
        ~PPU() = default;

        void Update(uint32_t cycles);
        [[nodiscard]] const globals::LCD_BUFFER& getLCDBuffer() const;
        [[nodiscard]] globals::TILE_DATA_BUFFER getTileDataBuffer();

        void WriteVRAM(uint16_t address, uint8_t value);
        void WriteOAM(uint16_t address, uint8_t value);
        void WriteIO(uint16_t address, uint8_t value);

        [[nodiscard]] uint8_t ReadVRAM(uint16_t address) const;
        [[nodiscard]] uint8_t ReadOAM(uint16_t address) const;
        [[nodiscard]] uint8_t ReadIO(uint16_t address) const;
    private:
        struct FIFO_Pixel
        {

        };

        void SetMode(PPUMode mode);
        void EnterMode(PPUMode mode);
        void UpdateMode(PPUMode mode);
        void ExitMode(PPUMode mode);

        Tile GetTileWindowBG(uint32_t index) const;

        InterruptController& m_interrupt_controller;

        // IO Registers
        LCDCRegister LCDC {};
        STATRegister STAT {};
        ByteRegister SCY {};
        ByteRegister SCX {};
        ByteRegister LY {};
        ByteRegister LYC {};
        ByteRegister DMA {};
        ByteRegister BGP {};
        ByteRegister OBP0 {};
        ByteRegister OBP1 {};
        ByteRegister WY {};
        ByteRegister WX {};

        std::array<uint8_t, globals::VRAM.m_size> VRAM {};
        std::array<uint8_t, globals::OAM.m_size> OAM {};

        FIFO m_fifo {};

        uint32_t fetcher_x = 0;
        uint32_t fetcher_y = 0;

        globals::LCD_BUFFER m_lcd {};
        globals::TILE_DATA_BUFFER m_tile_data_buffer {};

        uint32_t m_current_cycles = 0;
        PPUMode m_mode = PPUMode::NONE;
    };
}
