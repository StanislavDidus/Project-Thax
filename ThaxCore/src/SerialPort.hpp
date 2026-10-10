#pragma once
#include <cstdint>

#include "Utils/Math.hpp"

namespace dmg
{
    class SerialPort
    {
    public:
        SerialPort() = default;
        ~SerialPort() = default;

        void Update(uint32_t cycles)
        {
            if (IsTransferEnabled())
            {
                m_transfer_progress += cycles;
                SB = SB << cycles;
            }
        }

        void WriteSB(const uint8_t value) { SB = value; }
        [[nodiscard]] uint8_t ReadSB() const { return SB; }

        void WriteSC(const uint8_t value) { SC = value; }
        [[nodiscard]] uint8_t ReadSC() const { return SC; }

    private:
        [[nodiscard]] bool IsTransferEnabled() const { return getBit(SC, 7); }

        uint8_t m_transfer_progress = 0;

        uint8_t SB = 0;
        uint8_t SC = 0;
    };
}
