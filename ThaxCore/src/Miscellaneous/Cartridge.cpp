#include "Cartridge.hpp"

namespace dmg
{
    Cartridge::Cartridge()
    {
        ROM.resize(globals::ROM_0.m_size + globals::ROM_N.m_size);
    }

    void Cartridge::Load(const std::filesystem::path& path)
    {
        // Reset ROM
        std::fill_n(ROM.data(), ROM.size(), 0);

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            throw std::runtime_error{"Could not open file for reading."};
        }

        std::streamsize file_size = file.tellg();
        ROM.resize(file_size);

        file.seekg(0, std::ios::beg);

        file.read(reinterpret_cast<char*>(ROM.data()), file_size);

        if (file.bad())
        {
            throw std::runtime_error{"Failed to ream from a file"};
        }

        DMG_INFO("ROM loaded: {}", path.filename().string());
    }

    void Cartridge::LoadBootrom(const std::filesystem::path& path)
    {
        std::fill_n(BOOTROM.data(), BOOTROM.size(), 0);

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            throw std::runtime_error{"Could not open file for reading."};
        }

        std::streamsize file_size = file.tellg();

        assert(file_size == 256);

        file.seekg(0, std::ios::beg);

        file.read(reinterpret_cast<char*>(BOOTROM.data()), file_size);

        if (file.bad())
        {
            throw std::runtime_error{"Failed to ream from a file"};
        }

        m_bootrom_mapped = true;

        DMG_INFO("BOOTROM loaded: {}", path.filename().string());
    }

    void Cartridge::UnmapBootrom()
    {
        m_bootrom_mapped = false;
    }

    uint8_t Cartridge::Read(const uint16_t address) const
    {
        assert(address < globals::ROM_0.m_size + globals::ROM_N.m_size);

        if (address < 0x100 && m_bootrom_mapped)
            return BOOTROM[address];

        return ROM[address];
    }

    void Cartridge::Write(uint16_t address, const uint8_t value)
    {
        //ROM[address] = value;
    }
}
