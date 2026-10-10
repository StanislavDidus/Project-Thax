#pragma once

#include <cstdint>

#include "InterruptController.hpp"
#include "Utils/Math.hpp"

namespace dmg
{
    class Timer
    {
    public:
        Timer(InterruptController& interrupt_controller);
        ~Timer() = default;

        void Update(uint32_t cycles);

        [[nodiscard]] uint8_t ReadDIV() const { return DIV >> 6 & 0xFF; }
        void WriteDIV(const uint8_t) { DIV = 0; }

        [[nodiscard]] uint8_t ReadTIMA() const { return TIMA; }
        void WriteTIMA(const uint8_t value) { TIMA = value; }

        [[nodiscard]] uint8_t ReadTMA() const { return TMA; }
        void WriteTMA(const uint8_t value) { TMA = value; }

        [[nodiscard]] uint8_t ReadTAC() const { return TAC; }
        void WriteTAC(const uint8_t value);

    private:
        static constexpr uint32_t DIV_CLOCK_FREQUENCY = 256;

        [[nodiscard]] bool IsClockEnabled() const { return getBit(TAC, 2); }
        [[nodiscard]] uint8_t GetSelectedBit() const;
        void IncrementTima();

        uint16_t DIV = 0;
        uint8_t TIMA = 0;
        uint8_t TMA = 0;
        uint8_t TAC = 0;

        uint8_t old_bit_value = 0;

        InterruptController& m_interrupt_controller;
    };
}
