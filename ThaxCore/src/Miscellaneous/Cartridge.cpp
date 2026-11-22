#include "Cartridge.hpp"

#include <cassert>

#include "MBC/MBC.hpp"
#include "MBC/NOMBC.hpp"
#include "MBC/MBC1.hpp"
#include "MBC/MBC2.hpp"
#include "MBC/MBC3.hpp"

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

        ResizeROM();
        ResizeRAM();
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
        if (address < 0x100 && m_bootrom_mapped)
            return BOOTROM[address];

        if (MBC)
            return MBC->ReadROM(address);

        return globals::GARBAGE;
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

        return globals::GARBAGE;
    }

    void Cartridge::WriteRAM(const uint16_t address, const uint8_t value)
    {
        if (MBC)
            MBC->WriteRAM(address, value);
    }

    void Cartridge::CreateMBC()
    {
        switch (static_cast<CARTRIDGE_TYPE>(ROM[0x147]))
        {
        case CARTRIDGE_TYPE::ROM_ONLY:
            MBC = std::make_unique<NOMBC>(ROM, RAM);
            break;
        case CARTRIDGE_TYPE::MBC1:
            MBC = std::make_unique<MBC1>(ROM, RAM, MBC::C_NONE);
            break;
        case CARTRIDGE_TYPE::MBC1_RAM:
            MBC = std::make_unique<MBC1>(ROM, RAM, MBC::C_RAM);
            break;
        case CARTRIDGE_TYPE::MBC1_RAM_BATTERY:
            MBC = std::make_unique<MBC1>(ROM, RAM, MBC::C_RAM | MBC::C_BATTERY);
            break;
        case CARTRIDGE_TYPE::MBC2:
            MBC = std::make_unique<MBC2>(ROM, RAM, MBC::C_NONE);
            break;
        case CARTRIDGE_TYPE::MBC2_BATTERY:
            MBC = std::make_unique<MBC2>(ROM, RAM, MBC::C_BATTERY);
            break;
        case CARTRIDGE_TYPE::MBC3_TIMER_BATTERY:
            MBC = std::make_unique<MBC3>(ROM, RAM, MBC::C_TIMER | MBC::C_BATTERY);
            break;
        case CARTRIDGE_TYPE::MBC3_TIMER_RAM_BATTERY:
            MBC = std::make_unique<MBC3>(ROM, RAM, MBC::C_TIMER | MBC::C_RAM | MBC::C_BATTERY);
            break;
        case CARTRIDGE_TYPE::MBC3:
            MBC = std::make_unique<MBC3>(ROM, RAM, MBC::C_NONE);
            break;
        case CARTRIDGE_TYPE::MBC3_RAM:
            MBC = std::make_unique<MBC3>(ROM, RAM, MBC::C_RAM);
            break;
        case CARTRIDGE_TYPE::MBC3_RAM_BATTERY:
            MBC = std::make_unique<MBC3>(ROM, RAM, MBC::C_RAM | MBC::C_BATTERY);
            break;

        default: DMG_ERROR("This MBC type is not supported.") ;
        }
    }

    void Cartridge::ResizeROM()
    {
        byte rom_size {0_b};

        switch (static_cast<ROM_SIZE>(ROM[0x148]))
        {
        case ROM_SIZE::BANKS_2:   rom_size = byte{32_kib};  break;
        case ROM_SIZE::BANKS_4:   rom_size = byte{64_kib};  break;
        case ROM_SIZE::BANKS_8:   rom_size = byte{128_kib}; break;
        case ROM_SIZE::BANKS_16:  rom_size = byte{256_kib}; break;
        case ROM_SIZE::BANKS_32:  rom_size = byte{512_kib}; break;
        case ROM_SIZE::BANKS_64:  rom_size = byte{1_mib};   break;
        case ROM_SIZE::BANKS_128: rom_size = byte{2_mib};   break;
        case ROM_SIZE::BANKS_256: rom_size = byte{4_mib};   break;
        case ROM_SIZE::BANKS_512: rom_size = byte{8_mib};   break;
        default: DMG_ERROR("Unknown rom size");
        }

        ROM.resize(static_cast<size_t>(rom_size));

    }

    void Cartridge::ResizeRAM()
    {
        byte ram_size {0_b};

        switch (static_cast<RAM_SIZE>(ROM[0x149]))
        {
        case RAM_SIZE::NO_RAM:   ram_size = byte{0_b};     break;
        case RAM_SIZE::BANK_1:   ram_size = byte{8_kib};   break;
        case RAM_SIZE::BANK_4:   ram_size = byte{32_kib};  break;
        case RAM_SIZE::BANK_8:   ram_size = byte{64_kib};  break;
        case RAM_SIZE::BANK_16:  ram_size = byte{128_kib}; break;
        default: DMG_ERROR("Unknown ram size");
        }

        RAM.resize(static_cast<size_t>(ram_size));
    }
}
