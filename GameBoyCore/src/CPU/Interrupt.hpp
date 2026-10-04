#pragma once
#include <cstdint>
#include <optional>
#include <stdexcept>

#include "DMG/Utils/Log.hpp"

namespace dmg
{
    class Interrupt
    {
    public:
         Interrupt(Bus& bus, uint16_t address, bool enable_last_bits)
        : m_bus(bus)
        , m_address(address)
        , m_enable_last_bits(enable_last_bits)
        {

        }
        ~Interrupt() = default;

        void Set(const uint8_t value)
        {
            m_bus.Write(m_address, m_bus.Read(m_address) | value);
        }

        void Reset(const uint8_t value)
        {
            m_bus.Write(m_address, m_bus.Read(m_address) & ~value);
        }

        bool ReadBit(uint8_t i) const
        {
            // Bounds checking
            if (i < 0 || i > 7)
            {
                DMG_ERROR("Trying to read bit {} from an interrupt.", i);
                throw std::runtime_error{"Trying to read a wrong bit from an interrupt."};
            }

            // In Interrupt Flag (IF) Last 3 bits always return "1" when read
            if (m_enable_last_bits && i > 4)
                return true;

            return m_bus.Read(m_address) >> i & 1;
        }

        std::optional<uint8_t> GetSharedBit(const Interrupt& interrupt)
        {
            // NOTE: We don't have to go through all 8 bits because
            // the last three are not used.
            for (int i = 0; i < 5; ++i)
            {
                if (ReadBit(i) && interrupt.ReadBit(i))
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
        Bus& m_bus;
        uint16_t m_address = 0;
        bool m_enable_last_bits = false;
    };
}
