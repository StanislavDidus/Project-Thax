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
        Cartridge() = default;
        ~Cartridge() = default;

        void Load(const std::filesystem::path& path)
        {
            // Reset ROM
            std::fill_n(ROM.data(), ROM.size(), 0);

            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file)
            {
                throw std::runtime_error{"Could not open file for reading."};
            }

            std::streamsize file_size = file.tellg();

            assert(file_size <= 32768);

            file.seekg(0, std::ios::beg);

            file.read(reinterpret_cast<char*>(ROM.data()), file_size);

            if (file.bad())
            {
                throw std::runtime_error{"Failed to ream from a file"};
            }

            DMG_INFO("ROM loaded: {}", path.filename().string());
        }

        uint8_t Read(uint16_t address) const
        {
            return ROM[address];
        }

        void Write(uint16_t address, uint8_t value)
        {
            ROM[address] = value;
        }

    private:
        std::array<uint8_t, globals::ROM_0.m_size + globals::ROM_N.m_size> ROM {};
    };
}
