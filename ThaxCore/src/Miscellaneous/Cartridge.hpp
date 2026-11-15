#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>

#include "DMG/Utils/Log.hpp"
#include "../InternalGlobals.hpp"

namespace dmg
{
    class MBC;

    class Cartridge
    {
    public:
        Cartridge();
        ~Cartridge();

        void Load(const std::filesystem::path& path);
        void LoadBootrom(const std::filesystem::path& path);

        void UnmapBootrom();

        void Reset();

        [[nodiscard]] uint8_t ReadROM(uint16_t address) const;
        void WriteROM(uint16_t address, uint8_t value);

        [[nodiscard]] uint8_t ReadRAM(uint16_t address) const;
        void WriteRAM(uint16_t address, uint8_t value);

    private:
        void CreateMBC();

        std::vector<uint8_t> ROM {};
        std::vector<uint8_t> RAM {};
        std::array<uint8_t, globals::BOOTROM_SIZE> BOOTROM {};
        bool m_bootrom_mapped = false;
        bool m_is_ram_enabled = false;

        int m_rom_bank = 1;
        int m_ram_bank = 0;
        int m_banking_mode = 0;

        std::string m_game_name {};

        std::unique_ptr<MBC> MBC;
    };
}
