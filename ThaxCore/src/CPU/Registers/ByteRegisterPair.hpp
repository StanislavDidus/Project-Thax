#pragma once

#include "ATwoByteRegister.hpp"
#include "AByteRegister.hpp"

namespace dmg
{
    class ByteRegisterPair final : public ATwoByteRegister
    {
    public:
        ByteRegisterPair(AByteRegister& high, AByteRegister& low)
            : m_high(high)
            , m_low(low) {}
        ~ByteRegisterPair() override = default;

        [[nodiscard]] uint8_t ReadLow() const override { return m_low.Read(); }
        [[nodiscard]] uint8_t ReadHigh() const override { return m_high.Read(); }
        [[nodiscard]] uint16_t ReadWord() const override { return (m_high.Read() << 8) | m_low.Read(); }

        void WriteLow(const uint8_t data) override { m_low.Write(data); }
        void WriteHigh(const uint8_t data) override { m_high.Write(data); }
        void WriteWord(const uint16_t data) override { m_high.Write(data >> 8); m_low.Write(data & 0xFF); }

        //uint16_t increment() override { uint16_t temp = readWord(); writeWord(readWord() + 1); return temp; }
        //uint16_t decrement() override { writeWord(readWord() - 1); return readWord();}

        uint16_t operator++()
        {
            WriteWord(ReadWord() + 1);
            return ReadWord();
        }

        uint16_t operator++(int)
        {
            uint16_t temp = ReadWord();
            WriteWord(ReadWord() + 1);
            return temp;
        }

        uint16_t operator--()
        {
            WriteWord(ReadWord() - 1);
            return ReadWord();
        }

        uint16_t operator--(int)
        {
            uint16_t temp = ReadWord();
            WriteWord(ReadWord() - 1);
            return temp;
        }
    private:
        AByteRegister& m_high;
        AByteRegister& m_low;
    };
}