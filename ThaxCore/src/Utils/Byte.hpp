#pragma once
#include <cmath>

namespace dmg
{
    struct byte
    {
        constexpr byte() noexcept = default;
        constexpr explicit byte(const int data) noexcept : m_data(data) {}

        explicit operator int() const
        {
           return m_data;
        }

        explicit operator size_t() const
        {
            return static_cast<size_t>(m_data);
        }

        friend bool operator==(const byte& lhs, const byte& rhs)
        {
            return lhs.m_data == rhs.m_data;
        }

        friend bool operator!=(const byte& lhs, const byte& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator<(const byte& lhs, const byte& rhs)
        {
            return lhs.m_data < rhs.m_data;
        }

        friend bool operator>(const byte& lhs, const byte& rhs)
        {
            return lhs.m_data > rhs.m_data;
        }

        friend bool operator<=(const byte& lhs, const byte& rhs)
        {
            return lhs.m_data <= rhs.m_data;
        }

        friend bool operator>=(const byte& lhs, const byte& rhs)
        {
            return lhs.m_data >= rhs.m_data;
        }

        int m_data = 0;
    };

    inline byte operator""_b(const unsigned long long b)
    {
        return byte{static_cast<int>(b)};
    }

    inline byte operator""_kib(const unsigned long long kib)
    {
        return byte{static_cast<int>(kib * std::pow(2,10))};
    }

    inline byte operator""_mib(const unsigned long long mib)
    {
        return byte{static_cast<int>(mib * std::pow(2,20))};
    }
}
