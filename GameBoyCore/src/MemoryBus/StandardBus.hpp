#pragma once
#include <array>

#include "Bus.hpp"
#include "Cartridge.hpp"
#include "Constants.hpp"

namespace dmg
{
    class StandardBus : public Bus
    {
    public:
        StandardBus(Cartridge& cartridge, InterruptController& interrupt_controller)
            : m_cartridge(cartridge)
            , m_interrupt_controller(interrupt_controller)
        {

        }
        ~StandardBus() override = default;

        void Clear() override
        {
            for (int i = 0; i < address::RAM_SIZE; ++i)
            {
                Write(i, 0);
            }
        }

        [[nodiscard]] uint8_t Read(const uint16_t address) const override
        {
            if (address::ROM_0.Contains(address))
            {
                return m_cartridge.Read(address);
            }
            if (address::ROM_N.Contains(address))
            {
                return m_cartridge.Read(address);
            }
            if (address::VRAM.Contains(address))
            {
                return VRAM[address - address::VRAM.m_start];
            }
            if (address::RAM_N.Contains(address))
            {
                return RAM_N[address - address::RAM_N.m_start];
            }
            if (address::WRAM.Contains(address))
            {
               return WRAM[address - address::WRAM.m_start];
            }
            if (address::ECHO.Contains(address))
            {
               return WRAM[address - address::ECHO.m_start];
            }
            if (address::OAM.Contains(address))
            {
                return OAM[address - address::OAM.m_start];
            }
            if (address::UNSUABLE1.Contains(address))
            {
                return 0xFF;
            }
            if (address::IO.Contains(address))
            {
                if (address == 0xFF0F) return m_interrupt_controller.ReadIF();
                else return IO[address - address::IO.m_start];
            }
            if (address::UNUSABLE2.Contains(address))
            {
                return 0xFF;
            }
            if (address::IE.Contains(address))
            {
                return m_interrupt_controller.ReadIE();
            }

            return 0x00;
        }

        void Write(const uint16_t address, const uint8_t value) override
        {
            AddDirtyAddress(address);

            if (address::ROM_0.Contains(address))
            {
                m_cartridge.Write(address, value);
            }
            else if (address::ROM_N.Contains(address))
            {
                m_cartridge.Write(address, value);
            }
            else if (address::VRAM.Contains(address))
            {
                VRAM[address - address::VRAM.m_start] = value;
            }
            else if (address::RAM_N.Contains(address))
            {
                RAM_N[address - address::RAM_N.m_start] = value;
            }
            else if (address::WRAM.Contains(address))
            {
               WRAM[address - address::WRAM.m_start] = value;
            }
            else if (address::ECHO.Contains(address))
            {
                WRAM[address - address::ECHO.m_start] = value;
            }
            else if (address::OAM.Contains(address))
            {
                OAM[address - address::OAM.m_start] = value;
            }
            else if (address::UNSUABLE1.Contains(address))
            {
                //
            }
            else if (address::IO.Contains(address))
            {
                if (address == 0xFF0F) m_interrupt_controller.WriteIF(value);
                else IO[address - address::IO.m_start] = value;
            }

            else if (address::UNUSABLE2.Contains(address))
            {
                //
            }
            else if (address::IE.Contains(address))
            {
                m_interrupt_controller.WriteIF(value);
            }
        }
    private:
        Cartridge& m_cartridge;
        InterruptController& m_interrupt_controller;

        std::array<uint8_t, address::VRAM.m_size> VRAM   {};
        std::array<uint8_t, address::RAM_N.m_size> RAM_N {};
        std::array<uint8_t, address::WRAM.m_size> WRAM   {};
        std::array<uint8_t, address::OAM.m_size> OAM     {};
        std::array<uint8_t, address::IO.m_size> IO       {};
        std::array<uint8_t, address::HRAM.m_size> HRAM   {};
    };
}
