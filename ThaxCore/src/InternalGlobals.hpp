#pragma once

#include <cstdint>

namespace dmg
{
#ifndef NDEBUG
    inline constexpr bool DEBUG_MODE = true;
    inline constexpr bool PRINT_SERIAL_PORT = true;
    inline constexpr bool WRITE_DOCTOR_LOGS = false;
#else
    inline constexpr bool DEBUG_MODE = false;
    inline constexpr bool PRINT_SERIAL_PORT = false;
    inline constexpr bool WRITE_DOCTOR_LOGS = false;
#endif

}

namespace dmg
{
    struct Region
    {
        constexpr Region(uint32_t start, uint32_t size)
            : m_start(start)
            , m_size(size) {}

        [[nodiscard]] constexpr uint32_t End() const { return m_start + m_size; }
        [[nodiscard]] constexpr bool Contains(const uint32_t address) const { return address >= m_start && address < End(); }

        uint32_t m_start = 0;
        uint32_t m_size = 0;
    };

    enum class ObjectSize : uint8_t
    {
        SIZE_8X8,
        SIZE_8X16,
    };

    enum class ObjectPalette : uint8_t
    {
        OBP0,
        OBP1,
    };

    enum class PPUMode : uint8_t
    {
        NONE,
        MODE0,
        MODE1,
        MODE2,
        MODE3,
    };

    enum class PixelSource : uint8_t
    {
        BG,
        OBP0,
        OBP1,
    };
}

namespace dmg::globals
{
    // PPU MODES
    inline constexpr uint32_t PPU_MODE_0_LENGTH = 204;
    inline constexpr uint32_t PPU_MODE_1_LENGTH = 4560;
    inline constexpr uint32_t PPU_MODE_2_LENGTH = 80;
    inline constexpr uint32_t PPU_MODE_3_LENGTH = 172;

    inline constexpr uint32_t TILE_BYTE_SIZE = 16;
    inline constexpr uint32_t TILE_PIXEl_SIZE = 8;
    inline constexpr uint32_t TILEMAP_TILE_SIZE = 32;

    inline constexpr uint32_t TILE_REGION_SIZE = 0x1800;
    inline constexpr Region TILE_BLOCK_0 {0x0000, 0x800};
    inline constexpr Region TILE_BLOCK_1 {0x0800, 0x800};
    inline constexpr Region TILE_BLOCK_2 {0x1000, 0x800};
    inline constexpr Region TILE_REGION {0x8000, TILE_REGION_SIZE};

    static_assert(TILE_BLOCK_0.m_size +
                  TILE_BLOCK_1.m_size +
                  TILE_BLOCK_2.m_size == TILE_REGION_SIZE);

    // TILEMAP_ARE are the coordinates inside of the VRAM memory
    inline constexpr uint32_t TILEMAP_AREA_SIZE = 0x400;
    inline constexpr Region TILEMAP_AREA_1 {0x1800, 0x400};
    inline constexpr Region TILEMAP_AREA_2 {0x1C00, 0x400};
    ////////////////////////////////////////////////////////////

    inline constexpr uint8_t COLOR_WHITE = 0b00;
    inline constexpr uint8_t COLOR_LIGHT_GRAY = 0b01;
    inline constexpr uint8_t COLOR_DARK_GRAY = 0b10;
    inline constexpr uint8_t COLOR_BLACk = 0b11;

    inline constexpr uint32_t MAX_OBJECTS = 40;
    inline constexpr uint32_t MAX_OBJECTS_PER_SCANLINE = 10;

    inline constexpr uint32_t RAM_SIZE = 0x10000;

    inline constexpr Region ROM_0 {0x0000, 0x4000};
    inline constexpr Region ROM_N {0x4000, 0x4000};
    inline constexpr Region VRAM {0x8000, 0x2000};
    inline constexpr Region RAM_N {0xA000, 0x2000};
    inline constexpr Region WRAM {0xC000, 0x2000};
    inline constexpr Region ECHO {0xE000, 0x1E00};
    inline constexpr Region OAM {0xFE00, 0x00A0};
    inline constexpr Region UNUSABLE {0xFEA0, 0x0060};
    inline constexpr Region IO {0xFF00, 0x0080};
    inline constexpr Region PPU_IO {0xFF40, 12};

    inline constexpr Region HRAM {0xFF80, 0x007F};
    inline constexpr Region IE {0xFFFF, 0x0001};

    static_assert(ROM_N.m_start == ROM_0.End());
    static_assert(VRAM.m_start == ROM_N.End());
    static_assert(RAM_N.m_start == VRAM.End());
    static_assert(WRAM.m_start == RAM_N.End());
    static_assert(ECHO.m_start == WRAM.End());
    static_assert(OAM.m_start == ECHO.End());
    static_assert(UNUSABLE.m_start == OAM.End());
    static_assert(IO.m_start == UNUSABLE.End());
    static_assert(HRAM.m_start == IO.End());
    static_assert(IE.m_start == HRAM.End());
    static_assert(IE.End() == RAM_SIZE);
    static_assert(ROM_0.m_size +
                  ROM_N.m_size +
                  VRAM.m_size +
                  RAM_N.m_size +
                  WRAM.m_size +
                  ECHO.m_size +
                  OAM.m_size +
                  UNUSABLE.m_size +
                  IO.m_size +
                  HRAM.m_size +
                  IE.m_size == RAM_SIZE);
}