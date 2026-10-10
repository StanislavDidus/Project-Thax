#pragma once

#include "ByteRegister.hpp"

namespace dmg
{
    class FlagRegister final : public AByteRegister
    {
    public:
        FlagRegister() = default;
        ~FlagRegister() override = default;

        [[nodiscard]] uint8_t Read() const override { return m_data; }
        void Write(const uint8_t data) override{ m_data = data & 0xF0; /*Mask the lower 4 bits, they are always 0.*/ }

        void SetFlagZ(bool value) { m_data = value ? m_data | (1u << 7) : m_data & ~(1u << 7); }
        void SetFlagN(bool value) { m_data = value ? m_data | (1u << 6) : m_data & ~(1u << 6); }
        void SetFlagH(bool value) { m_data = value ? m_data | (1u << 5) : m_data & ~(1u << 5); }
        void SetFlagC(bool value) { m_data = value ? m_data | (1u << 4) : m_data & ~(1u << 4); }

        [[nodiscard]] uint8_t ReadFlagZ() const { return (m_data & 0b10000000) >> 7; }
        [[nodiscard]] uint8_t ReadFlagN() const { return (m_data & 0b01000000) >> 6;}
        [[nodiscard]] uint8_t ReadFlagH() const { return (m_data & 0b00100000) >> 5;}
        [[nodiscard]] uint8_t ReadFlagC() const { return (m_data & 0b00010000) >> 4;}
    private:
        uint8_t m_data = 0;

    };
}
