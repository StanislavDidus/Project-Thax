#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>

#include "DMG/Utils/Log.hpp"
#include "../InternalGlobals.hpp"

namespace dmg
{
    class Cartridge
    {
    public:
        Cartridge();
        ~Cartridge() = default;

        void Load(const std::filesystem::path& path);
        void LoadBootrom(const std::filesystem::path& path);

        void UnmapBootrom();

        [[nodiscard]] uint8_t Read(uint16_t address) const;
        void Write(uint16_t address, uint8_t value);

    private:
        std::vector<uint8_t> ROM {};
        std::array<uint8_t, 256> BOOTROM {};
        bool m_bootrom_mapped = false;
    };
}
