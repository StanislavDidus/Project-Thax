#pragma once
#include <cstdint>

#include "InternalGlobals.hpp"

namespace dmg
{
    class MBC
    {
    public:
        MBC(const CARTRIDGE_TYPE cartridge_type, const ROM_SIZE rom_size, const RAM_SIZE ram_size)
            : m_cartridge_type(cartridge_type)
            , m_rom_size(rom_size)
            , m_ram_size(ram_size)
        {
            RAM.resize(globals::EXTERNAL_RAM.m_size);
        }
        virtual ~MBC() = default;

        virtual void WriteROM(uint16_t address, uint8_t value) = 0;
        virtual void WriteRAM(uint16_t address, uint8_t value) = 0;
        [[nodiscard]] virtual uint8_t ReadRAM(uint16_t address) const = 0;

        [[nodiscard]] uint32_t GetActiveROMBank() const { return m_active_rom_bank; }
        [[nodiscard]] uint32_t GetActiveRAMBank() const { return m_active_ram_bank; }
    protected:
        void SetActiveROMBank(const uint32_t value) { m_active_rom_bank = value; }
        void SetActiveRAMBank(const uint32_t value) { m_active_ram_bank = value; }

        [[nodiscard]] CARTRIDGE_TYPE GetCartridgeType() const { return m_cartridge_type; }
        [[nodiscard]] ROM_SIZE GetROMSize() const { return m_rom_size; }
        [[nodiscard]] RAM_SIZE GetRAMSize() const { return m_ram_size; }

        std::vector<uint8_t> RAM {};
    private:
        uint32_t m_active_rom_bank = 1;
        uint32_t m_active_ram_bank = 1;

        CARTRIDGE_TYPE m_cartridge_type;
        ROM_SIZE m_rom_size;
        RAM_SIZE m_ram_size;
    };
}
