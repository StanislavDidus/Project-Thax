#pragma once
#include <cstdint>

namespace dmg
{
    class ATwoByteRegister
    {
    public:
        ATwoByteRegister() = default;
        virtual ~ATwoByteRegister() = default;

        [[nodiscard]] virtual uint8_t ReadLow() const = 0;
        [[nodiscard]] virtual uint8_t ReadHigh() const = 0;
        [[nodiscard]] virtual uint16_t ReadWord() const = 0;

        virtual void WriteLow(uint8_t data) = 0;
        virtual void WriteHigh(uint8_t data) = 0;
        virtual void WriteWord(uint16_t data) = 0;

        virtual uint16_t operator++() = 0;
        virtual uint16_t operator++(int) = 0;
        virtual uint16_t operator--() = 0;
        virtual uint16_t operator--(int) = 0;
    };
}