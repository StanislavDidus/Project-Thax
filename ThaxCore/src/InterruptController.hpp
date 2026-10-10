#pragma once

#include <cstdint>
#include <optional>

#include "Utils/Math.hpp"

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

        [[nodiscard]] std::optional<uint8_t> GetSharedBit() const
        {
            // NOTE: We don't have to go through all 8 bits because
            // the last three are not used.
            for (int i = 0; i < 5; ++i)
            {
                if (getBit(IF, i) && getBit(IE, i))
                {
                    return i;
                }
            }

            return std::nullopt;
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
