#pragma once
#include <cstdint>

#include "ATwoByteRegister.hpp"

namespace dmg
{
    class TwoByteRegister final : public ATwoByteRegister
    {
    public:
        TwoByteRegister() = default;
        explicit  TwoByteRegister(const uint16_t data)
            : m_data(data) {}
        ~TwoByteRegister() override = default;

        [[nodiscard]] uint8_t ReadLow() const override { return m_data & 0xFF; }
        [[nodiscard]] uint8_t ReadHigh() const override { return m_data >> 8; }
        [[nodiscard]] uint16_t ReadWord() const override { return m_data; }

        void WriteLow(const uint8_t data) override { m_data = (ReadHigh() << 8) | data; }
        void WriteHigh(const uint8_t data) override { m_data = ReadLow() | (data << 8); }
        void WriteWord(const uint16_t data) override { m_data = data; }

        void increase(uint16_t value) { m_data += value; }
        void JumpRelative(int8_t value) {m_data = static_cast<uint16_t>(static_cast<int32_t>(m_data) + value); }

        //uint16_t increment() override { return m_data++; }
        //uint16_t decrement() override { return --m_data; }

        uint16_t operator++() override { return ++m_data; }
        uint16_t operator++(int) override { return m_data++; }
        uint16_t operator--() override { return --m_data; }
        uint16_t operator--(int) override { return m_data--; }

    private:
        uint16_t m_data = 0;
    };
}