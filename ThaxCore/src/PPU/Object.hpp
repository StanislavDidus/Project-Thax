#pragma once
#include <array>
#include <cstdint>

#include "InternalGlobals.hpp"

namespace dmg
{
    class Object
    {
    public:
        Object() = default;
        ~Object() = default;

        void Write(uint16_t address, uint8_t value);

        // READS & GETTERS
        [[nodiscard]] uint8_t Read(uint8_t address) const;
        [[nodiscard]] uint8_t GetY() const;
        [[nodiscard]] uint8_t GetX() const;
        [[nodiscard]] uint8_t GetTileIndex() const;
        [[nodiscard]] uint8_t GetAttributes() const;
        [[nodiscard]] bool GetPriority() const;
        [[nodiscard]] bool IsYFlip() const;
        [[nodiscard]] bool IsXFlip() const;
        [[nodiscard]] ObjectPalette GetDMGPalette() const;
    private:
        uint8_t m_y          = 0;
        uint8_t m_x          = 0;
        uint8_t m_tile_index = 0;
        uint8_t m_attributes = 0;
    };
}
