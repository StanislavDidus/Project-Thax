#pragma once
#include <cstdint>

#include "MemoryBus/Bus.hpp"

namespace dmg
{
    class DIV
    {
    public :
        DIV(Bus& bus, uint16_t address) : m_bus(bus), m_address(address) {}
        ~DIV() = default;

        DIV(const DIV& other) = default;
        DIV& operator=(const DIV& other) = default;
        DIV(DIV&& other) noexcept = default;
        DIV& operator=(DIV&& other) noexcept = default;

        void Write(uint16_t)
        {
            m_bus.Write(0x00);
        }

        void Increment()
        {
            m_bus.Write(m_address, m_bus.Read(m_address) + 1);
        }

    private:
        Bus& m_bus;
        uint16_t m_address = 0;
    };
}
