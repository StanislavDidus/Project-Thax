#pragma once
#include <cstdint>

#include "InternalGlobals.hpp"
#include "Utils/Byte.hpp"

namespace dmg
{

    class MBC
    {
    public:
        static constexpr uint8_t C_NONE    = 0 << 0;
        static constexpr uint8_t C_RAM     = 1 << 0;
        static constexpr uint8_t C_BATTERY = 1 << 1;
        static constexpr uint8_t C_TIMER   = 1 << 2;
        static constexpr uint8_t C_RUMBLE  = 1 << 3;
        static constexpr uint8_t C_SENSOR  = 1 << 4;

        MBC(std::vector<uint8_t>& ROM, std::vector<uint8_t>& RAM, uint8_t components);
        virtual ~MBC() = default;

        virtual void WriteROM(uint16_t address, uint8_t value) = 0;
        virtual void WriteRAM(uint16_t address, uint8_t value) = 0;
        [[nodiscard]] virtual uint8_t ReadROM(uint16_t address) const = 0;
        [[nodiscard]] virtual uint8_t ReadRAM(uint16_t address) const = 0;
    protected:
        void CheckAllowedComponents(uint8_t allowed_components) const;

        [[nodiscard]] byte GetROMSize() const;
        [[nodiscard]] byte GetRAMSize() const;
        [[nodiscard]] bool HasComponent(uint8_t components) const;
        [[nodiscard]] static unsigned int GetROMBankAddress(unsigned int bank);

    protected:
        std::vector<uint8_t>& ROM;
        std::vector<uint8_t>& RAM;
    private:
        uint8_t m_components = 0;
    };
}
