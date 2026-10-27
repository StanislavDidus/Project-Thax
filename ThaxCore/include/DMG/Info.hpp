#pragma once

#include <cstdint>

namespace dmg
{
    struct CPUInfo
    {
        uint8_t F, A, B, C, D, E, H, L;
        uint16_t PC, SP;
        uint8_t IME;
        uint8_t CPU_State;
    };

    struct PPUInfo
    {
        uint8_t LCDC, STAT, SCY, SCX;
        uint8_t LY, LYC, WX, WY;
        uint8_t DMA, BGP, OBP0, OBP1;
    };

    struct DMGInfo
    {
        // CPU Registers
        uint8_t F, A, B, C, D, E, H, L;
        uint16_t PC, SP;

        // CPU State (0-Normal, 1-Halt, 2-Stop)
        uint8_t CPU_State;

        // Interrupts
        uint8_t IF, IE, IME;

        // Timers
        uint8_t DIV, TIMA, TMA, TAC;

        // Joypad
        uint8_t P1;

        // Serial Port
        uint8_t SB, SC;

        // PPU
        uint8_t LCDC, STAT, SCY, SCX;
        uint8_t LY, LYC, WX, WY;
        uint8_t DMA, BGP, OBP0, OBP1;
    };
}