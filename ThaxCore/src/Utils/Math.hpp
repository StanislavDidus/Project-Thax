#pragma once

#include <cstdint>

namespace dmg
{
    [[nodiscard]] inline uint16_t make_uint16_t(const uint8_t msb, const uint8_t lsb) noexcept
    {
        return (msb << 8u) | lsb;
    }

    template<class T, class = std::enable_if_t<std::is_integral_v<T>>>
    uint8_t getBit(const T& reg, uint8_t index)
    {
        T mask = 1 << index;
        return (reg & mask) >> index;
    }

    template<class T, class = std::enable_if_t<std::is_integral_v<T>>>
    void setBit(T& reg, uint8_t index, uint8_t value)
    {
        reg = value ? reg | (1u << index) : reg & ~(1u << index);
    }
}