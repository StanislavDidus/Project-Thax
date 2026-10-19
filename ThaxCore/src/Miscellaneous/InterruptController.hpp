#pragma once

#include <cstdint>
#include <optional>

#include "../Utils/Math.hpp"

namespace dmg
{
    class InterruptController
    {
    public:
        InterruptController() = default;
        ~InterruptController() = default;

        void WriteIF(const uint8_t value)
        {
            IF = value;
        }
        void WriteIE(const uint8_t value)
        {
            IE = value;
        }

        [[nodiscard]] uint8_t ReadIF() const
        {
            // Lower 5 bits represent interrupts, other 3 always return 1
            return IF | 0b11100000;
        }
        [[nodiscard]] uint8_t ReadIE() const
        {
            return IE;
        }

        void SetIF(const uint8_t value)
        {
            IF |= value;
        }

        void ResetIF(const uint8_t value)
        {
            IF &= ~value;
        }

        [[nodiscard]] uint8_t GetPendingInterrupt() const
        {
            return IF & IE & 0x1F;
        }

        static constexpr uint8_t VBLANK = 0b1;
        static constexpr uint8_t LCD    = 0b10;
        static constexpr uint8_t Timer  = 0b100;
        static constexpr uint8_t Serial = 0b1000;
        static constexpr uint8_t Joypad = 0b10000;
    private:
        uint8_t IF;
        uint8_t IE;
    };
}
