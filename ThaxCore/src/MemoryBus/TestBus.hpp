#pragma once
#include <array>

#include "Bus.hpp"
#include "Constants.hpp"

namespace dmg
{
    class TestBus : public Bus
    {
    public:
        TestBus() = default;
        ~TestBus() override = default;

        void Clear() override
        {
            m_data.fill(0);
        }

        [[nodiscard]] uint8_t Read(uint16_t address) const override
        {
            return m_data[address];
        }

        void Write(uint16_t address, uint8_t value) override
        {
            AddDirtyAddress(address);
            m_data[address] = value;
        }
    private:
        std::array<uint8_t, address::RAM_SIZE> m_data {0};
    };
}
