#pragma once

#include <cstdint>

namespace dmg
{
#ifndef NDEBUG
    inline constexpr bool DEBUG_MODE = true;
    inline constexpr bool PRINT_SERIAL_PORT = false;
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

    enum class PixelSource : uint8_t
    {
        BG,
        OBP0,
        OBP1,
    };

    enum class CARTRIDGE_TYPE : uint8_t
    {
        ROM_ONLY                       = 0x00,
        MBC1                           = 0x01,
        MBC1_RAM                       = 0x02,
        MBC1_RAM_BATTERY               = 0x03,
        MBC2                           = 0x05,
        MBC2_BATTERY                   = 0x06,
        ROM_RAM                        = 0x08,
        ROM_RAM_BATTERY                = 0x09,
        MMM01                          = 0x0B,
        MMM01_RAM                      = 0x0C,
        MMM01_RAM_BATTERY              = 0x0D,
        MBC3_TIMER_BATTERY             = 0x0F,
        MBC3_TIMER_RAM_BATTERY         = 0x10,
        MBC3                           = 0x11,
        MBC3_RAM                       = 0x12,
        MBC3_RAM_BATTERY               = 0x13,
        MBC5                           = 0x19,
        MBC5_RAM                       = 0x1A,
        MBC5_RAM_BATTERY               = 0x1B,
        MBC5_RUMBLE                    = 0x1C,
        MBC5_RUMBLE_RAM                = 0x1D,
        MBC5_RUMBLE_RAM_BATTERY        = 0x1E,
        MBC6                           = 0x20,
        MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
        POCKET_CAMERA                  = 0xFC,
        BANDAI_TAMA5                   = 0xFD,
        HuC3                           = 0xFE,
        HuC1_RAM_BATTERY               = 0xFF,
    };

    enum class ROM_SIZE : uint8_t
    {
        BANKS_2   = 0x00,
        BANKS_4   = 0x01,
        BANKS_8   = 0x02,
        BANKS_16  = 0x03,
        BANKS_32  = 0x04,
        BANKS_64  = 0x05,
        BANKS_128 = 0x06,
        BANKS_256 = 0x07,
        BANKS_512 = 0x08,
    };

    enum class RAM_SIZE : uint8_t
    {
        NO_RAM  = 0x00,
        BANK_1  = 0x02,
        BANK_4  = 0x03,
        BANK_16 = 0x04,
        BANK_8  = 0x05,
    };
}

namespace dmg::globals
{


    // -------------------- PPU ------------------------ //
    inline constexpr uint32_t HBLANK_LENGTH = 204;
    inline constexpr uint32_t VBLANK_LENGTH = 4560;
    inline constexpr uint32_t OAM_SCAN_LENGTH = 80;
    inline constexpr uint32_t DRAWING_PIXELS_LENGTH = 172;

    inline constexpr uint32_t TILE_BYTE_SIZE = 16;
    inline constexpr uint32_t TILE_PIXEl_SIZE = 8;
    inline constexpr uint32_t TILEMAP_TILE_SIZE = 32;

    inline constexpr uint8_t TILE_WIDTH = 8;
    inline constexpr uint8_t TILE_HEIGHT = 8;

    inline constexpr uint32_t TILE_REGION_SIZE = 0x1800;
    inline constexpr Region TILE_BLOCK_0 {0x0000, 0x800};
    inline constexpr Region TILE_BLOCK_1 {0x0800, 0x800};
    inline constexpr Region TILE_BLOCK_2 {0x1000, 0x800};
    inline constexpr Region TILE_REGION {0x8000, TILE_REGION_SIZE};

    inline constexpr int LX_START = 7;
    inline constexpr int OBJECT_X_OFFSET = 8;
    inline constexpr int OBJECT_Y_OFFSET = 16;
    inline constexpr int WINDOW_X_OFFSET = 16;

    inline constexpr uint32_t MAX_OBJECTS = 40;
    inline constexpr uint32_t MAX_OBJECTS_PER_SCANLINE = 10;

    // -------------------- VRAM TILEMAP AREAS ------------------------ //

    inline constexpr uint32_t TILEMAP_AREA_SIZE = 0x400;
    inline constexpr Region TILEMAP_AREA_1 {0x1800, 0x400};
    inline constexpr Region TILEMAP_AREA_2 {0x1C00, 0x400};

    // -------------------- COLORS ------------------------ //

    inline constexpr uint8_t COLOR_WHITE = 0b00;
    inline constexpr uint8_t COLOR_LIGHT_GRAY = 0b01;
    inline constexpr uint8_t COLOR_DARK_GRAY = 0b10;
    inline constexpr uint8_t COLOR_BLACk = 0b11;

    // -------------------- MBC CHIPS ------------------------ //

    inline constexpr Region MBC1_RAM_ENABLE {0x0000, 0x2000};
    inline constexpr Region MBC1_ROM_BANK_NUMBER {0x2000, 0x2000};
    inline constexpr Region MBC1_RAM_BANK_NUMBER {0x4000, 0x2000};
    inline constexpr Region MBC1_BANKING_MODE_SELECT {0x6000, 0x2000};

    inline constexpr int MBC2_RAM_SIZE = 512;
    inline constexpr Region MBC2_RAM_ENABLE {0x0000, 0x4000};

    // -------------------- MEMORY AND REGIONS ------------------------ //

    inline constexpr uint8_t GARBAGE = 0xFF;

    inline constexpr uint32_t RAM_SIZE = 0x10000;
    inline constexpr uint32_t ROM_SIZE = 0x8000;
    inline constexpr uint32_t BOOTROM_SIZE = 0x100;

    inline constexpr Region ROM_0 {0x0000, 0x4000};
    inline constexpr Region ROM_N {0x4000, 0x4000};
    inline constexpr Region VRAM {0x8000, 0x2000};
    inline constexpr Region EXTERNAL_RAM {0xA000, 0x2000};
    inline constexpr Region WRAM {0xC000, 0x2000};
    inline constexpr Region ECHO {0xE000, 0x1E00};
    inline constexpr Region OAM {0xFE00, 0x00A0};
    inline constexpr Region UNUSABLE {0xFEA0, 0x0060};
    inline constexpr Region IO {0xFF00, 0x0080};
    inline constexpr Region PPU_IO {0xFF40, 12};
    inline constexpr Region HRAM {0xFF80, 0x007F};
    inline constexpr Region IE {0xFFFF, 0x0001};

    // -------------------- ASSERTIONS ------------------------ //

    static_assert(ROM_N.m_start == ROM_0.End());
    static_assert(VRAM.m_start == ROM_N.End());
    static_assert(EXTERNAL_RAM.m_start == VRAM.End());
    static_assert(WRAM.m_start == EXTERNAL_RAM.End());
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
                  EXTERNAL_RAM.m_size +
                  WRAM.m_size +
                  ECHO.m_size +
                  OAM.m_size +
                  UNUSABLE.m_size +
                  IO.m_size +
                  HRAM.m_size +
                  IE.m_size == RAM_SIZE);

    static_assert(TILE_BLOCK_0.m_size +
                  TILE_BLOCK_1.m_size +
                  TILE_BLOCK_2.m_size == TILE_REGION_SIZE);

    static_assert(MBC1_RAM_ENABLE.m_size +
                  MBC1_ROM_BANK_NUMBER.m_size +
                  MBC1_RAM_BANK_NUMBER.m_size +
                  MBC1_BANKING_MODE_SELECT.m_size == ROM_SIZE);
}