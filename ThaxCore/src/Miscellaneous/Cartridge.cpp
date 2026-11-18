#include "Cartridge.hpp"

#include <cassert>

#include "MBC/MBC.hpp"
#include "MBC/NOMBC.hpp"
#include "MBC/MBC1.hpp"

namespace dmg
{
    Cartridge::Cartridge()
    {
        Reset();
    }

    Cartridge::~Cartridge()
    {
    }

    void Cartridge::Load(const std::filesystem::path& path)
    {
        // Reset ROM
        ROM.clear();

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            throw std::runtime_error{"Could not open file for reading."};
        }

        std::streamsize file_size = file.tellg();
        ROM.resize(std::max(static_cast<unsigned int>(file_size), globals::ROM_0.m_size + globals::ROM_N.m_size));

        file.seekg(0, std::ios::beg);

        file.read(reinterpret_cast<char*>(ROM.data()), file_size);

        if (file.bad())
        {
            throw std::runtime_error{"Failed to ream from a file"};
        }

        for (int i = 0x134; i <= 0x143; ++i)
            m_game_name += static_cast<char>(ROM[i]);

        CreateMBC();

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

    void Cartridge::Reset()
    {
        ROM.clear();
        RAM.clear();

        ROM.resize(globals::ROM_0.m_size + globals::ROM_N.m_size);
        RAM.resize(globals::EXTERNAL_RAM.m_size);

        m_bootrom_mapped = false;
        m_is_ram_enabled = true;
        m_rom_bank = 1;
        m_ram_bank = 1;
        m_banking_mode = 0;

        m_game_name = "";
    }

    uint8_t Cartridge::ReadROM(const uint16_t address) const
    {
        assert(address < globals::ROM_0.m_size + globals::ROM_N.m_size);

        if (!MBC) return 0xFF;

        if (address < 0x100 && m_bootrom_mapped)
            return BOOTROM[address];

        if (globals::ROM_0.Contains(address))
        {
            return ROM[address];
        }
        if (globals::ROM_N.Contains(address))
        {
            return ROM[address + (MBC->GetActiveROMBank() - 1) * globals::ROM_N.m_size];
        }

        return 0xFF;
    }

    void Cartridge::WriteROM(const uint16_t address, const uint8_t value)
    {
        if (MBC)
            MBC->WriteROM(address, value);
    }

    uint8_t Cartridge::ReadRAM(const uint16_t address) const
    {
        if (MBC)
            return MBC->ReadRAM(address);

        return 0xFF;
    }

    void Cartridge::WriteRAM(const uint16_t address, const uint8_t value)
    {
        if (MBC)
            MBC->WriteRAM(address, value);
    }

    void Cartridge::CreateMBC()
    {
        //const auto cartridge_type = static_cast<CARTRIDGE_TYPE>(ROM[0x147]);
        const auto cartridge_type = CARTRIDGE_TYPE::MBC1;
        const auto rom_size = static_cast<ROM_SIZE>(ROM[0x148]);
        const auto ram_size = static_cast<RAM_SIZE>(ROM[0x149]);

        switch (cartridge_type)
        {
        case CARTRIDGE_TYPE::ROM_ONLY:
            MBC = std::make_unique<NOMBC>(cartridge_type, rom_size, ram_size);
            break;
        case CARTRIDGE_TYPE::MBC1:
            MBC = std::make_unique<MBC1>(cartridge_type, rom_size, ram_size);
            break;
        default: DMG_ERROR("This MBC type is not supported.") ;
        }
    }
}
