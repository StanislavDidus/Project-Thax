#pragma once

#include <cstdint>

namespace dmg::address
{
    struct Region
    {
        constexpr Region(uint32_t start, uint32_t size)
            : m_start(start)
            , m_size(size) {}

        constexpr uint32_t End() const { return m_start + m_size; }
        constexpr bool Contains(const uint32_t address) const { return address >= m_start && address < End(); }

        uint32_t m_start = 0;
        uint32_t m_size = 0;
    };

    inline constexpr uint32_t RAM_SIZE = 0x10000;

    inline constexpr Region ROM_0 {0x0000, 0x4000};
    inline constexpr Region ROM_N {0x4000, 0x4000};
    inline constexpr Region VRAM {0x8000, 0x2000};
    inline constexpr Region RAM_N {0xA000, 0x2000};
    inline constexpr Region WRAM {0xC000, 0x2000};
    inline constexpr Region ECHO {0xE000, 0x1E00};
    inline constexpr Region OAM {0xFE00, 0x00A0};
    inline constexpr Region UNSUABLE1 {0xFEA0, 0x0060};
    inline constexpr Region IO {0xFF00, 0x004C};
    inline constexpr Region UNUSABLE2 {0xFF4C, 0x0034};
    inline constexpr Region HRAM {0xFF80, 0x007F};
    inline constexpr Region IE {0xFFFF, 0x0001};

    static_assert(ROM_N.m_start == ROM_0.End());
    static_assert(VRAM.m_start == ROM_N.End());
    static_assert(RAM_N.m_start == VRAM.End());
    static_assert(WRAM.m_start == RAM_N.End());
    static_assert(ECHO.m_start == WRAM.End());
    static_assert(OAM.m_start == ECHO.End());
    static_assert(UNSUABLE1.m_start == OAM.End());
    static_assert(IO.m_start == UNSUABLE1.End());
    static_assert(UNUSABLE2.m_start == IO.End());
    static_assert(HRAM.m_start == UNUSABLE2.End());
    static_assert(IE.m_start == HRAM.End());
    static_assert(IE.End() == RAM_SIZE);
    static_assert(ROM_0.m_size +
                  ROM_N.m_size +
                  VRAM.m_size +
                  RAM_N.m_size +
                  WRAM.m_size +
                  ECHO.m_size +
                  OAM.m_size +
                  UNSUABLE1.m_size +
                  IO.m_size +
                  UNUSABLE2.m_size +
                  HRAM.m_size +
                  IE.m_size == RAM_SIZE);
}