#pragma once
#include <array>
#include <cstdint>
#include <span>

namespace dmg
{
    class Object
    {
    public:
        Object(uint16_t address, std::span<const uint8_t> oam);
        ~Object() = default;

        [[nodiscard]] uint8_t GetYPosition () const;
        [[nodiscard]] uint8_t GetXPosition () const;
        [[nodiscard]] uint8_t GetTileIndex () const;
        [[nodiscard]] bool GetPriority     () const;
        [[nodiscard]] bool IsYFlipped      () const;
        [[nodiscard]] bool IsXFlipped      () const;
        [[nodiscard]] ObjectPalette GetPalette() const;

        bool operator<(const Object& other) const
        {
            if (GetXPosition() != other.GetXPosition())
                return GetXPosition() < other.GetXPosition();
            return m_address < other.m_address;
        }

    private:
        std::span<const uint8_t> m_oam;
        uint16_t m_address = 0;
    };
}
