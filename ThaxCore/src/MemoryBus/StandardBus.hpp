#pragma once
#include <array>

#include "Bus.hpp"
#include "Miscellaneous/Cartridge.hpp"
#include "../InternalGlobals.hpp"
#include "Miscellaneous/Timer.hpp"
#include "Miscellaneous/Joypad.hpp"
#include "Miscellaneous/SerialPort.hpp"

namespace dmg
{
    class StandardBus : public Bus
    {
    public:
        StandardBus(
            Cartridge& cartridge,
            InterruptController& interrupt_controller,
            Timer& timer,
            SerialPort& serial_port,
            Joypad& joypad,
            PPU& ppu
            )
            : m_cartridge(cartridge)
            , m_interrupt_controller(interrupt_controller)
            , m_timer(timer)
            , m_serial_port(serial_port)
            , m_joypad(joypad)
            , m_ppu(ppu)
        {

        }
        ~StandardBus() override = default;

        void Clear() override
        {
            for (int i = 0; i < globals::RAM_SIZE; ++i)
            {
                Write(i, 0);
            }
        }

        [[nodiscard]] uint8_t Read(const uint16_t address) const override
        {
            if (globals::ROM_0.Contains(address))
            {
                return m_cartridge.Read(address);
            }
            if (globals::ROM_N.Contains(address))
            {
                return m_cartridge.Read(address);
            }
            if (globals::VRAM.Contains(address))
            {
                return m_ppu.ReadVRAM(address - globals::VRAM.m_start);
            }
            if (globals::RAM_N.Contains(address))
            {
                return RAM_N[address - globals::RAM_N.m_start];
            }
            if (globals::WRAM.Contains(address))
            {
               return WRAM[address - globals::WRAM.m_start];
            }
            if (globals::ECHO.Contains(address))
            {
               return WRAM[address - globals::ECHO.m_start];
            }
            if (globals::OAM.Contains(address))
            {
                return m_ppu.ReadOAM(address - globals::OAM.m_start);
            }
            if (globals::UNUSABLE.Contains(address))
            {
                return 0xFF;
            }
            if (globals::IO.Contains(address))
            {
                if (globals::PPU_IO.Contains(address))
                {
                    return m_ppu.ReadIO(address - globals::PPU_IO.m_start);
                }

                switch (address)
                {
                case 0xFF00:
                    return m_joypad.Read();
                case 0xFF01:
                    return m_serial_port.ReadSB();
                case 0xFF02:
                    return m_serial_port.ReadSC();
                case 0xFF0F:
                    return m_interrupt_controller.ReadIF();
                case 0xFF04:
                    return m_timer.ReadDIV();
                case 0xFF05:
                    return m_timer.ReadTIMA();
                case 0xFF06:
                    return m_timer.ReadTMA();
                case 0xFF07:
                    return m_timer.ReadTAC();
                case 0xFF44:
                    return 0x90;
                default:
                    return IO[address - globals::IO.m_start];
                }
            }
            if (globals::HRAM.Contains(address))
            {
                return HRAM[address - globals::HRAM.m_start];
            }
            if (globals::IE.Contains(address))
            {
                return m_interrupt_controller.ReadIE();
            }

            return 0x00;
        }

        void Write(const uint16_t address, const uint8_t value) override
        {
            AddDirtyAddress(address);

            if (globals::ROM_0.Contains(address) || globals::ROM_N.Contains(address))
            {
            }
            else if (globals::VRAM.Contains(address))
            {
                m_ppu.WriteVRAM(address - globals::VRAM.m_start, value);
            }
            else if (globals::RAM_N.Contains(address))
            {
                RAM_N[address - globals::RAM_N.m_start] = value;
            }
            else if (globals::WRAM.Contains(address))
            {
               WRAM[address - globals::WRAM.m_start] = value;
            }
            else if (globals::ECHO.Contains(address))
            {
                WRAM[address - globals::ECHO.m_start] = value;
            }
            else if (globals::OAM.Contains(address))
            {
                m_ppu.WriteOAM(address - globals::OAM.m_start, value);
            }
            else if (globals::UNUSABLE.Contains(address))
            {
                //
            }
            else if (globals::IO.Contains(address))
            {
                if (globals::PPU_IO.Contains(address))
                {
                    m_ppu.WriteIO(address - globals::PPU_IO.m_start, value);
                }

                switch (address)
                {
                case 0xFF00:
                    m_joypad.Write(value);
                    break;
                case 0xFF01:
                    m_serial_port.WriteSB(value);
                    break;
                case 0xFF02:
                    m_serial_port.WriteSC(value);
                    break;
                case 0xFF0F:
                    m_interrupt_controller.WriteIF(value);
                    break;
                case 0xFF04:
                    m_timer.WriteDIV(value);
                    break;
                case 0xFF05:
                    m_timer.WriteTIMA(value);
                    break;
                case 0xFF06:
                    m_timer.WriteTMA(value);
                    break;
                case 0xFF07:
                    m_timer.WriteTAC(value);
                    break;
                default:
                    IO[address - globals::IO.m_start] = value;
                    break;
                }
            }
            else if (globals::HRAM.Contains(address))
            {
                HRAM[address - globals::HRAM.m_start] = value;
            }
            else if (globals::IE.Contains(address))
            {
                m_interrupt_controller.WriteIE(value);
            }
        }
    private:
        Cartridge& m_cartridge;
        InterruptController& m_interrupt_controller;
        Timer& m_timer;
        SerialPort& m_serial_port;
        Joypad& m_joypad;
        PPU& m_ppu;

        std::array<uint8_t, globals::RAM_N.m_size> RAM_N {};
        std::array<uint8_t, globals::WRAM.m_size> WRAM   {};
        std::array<uint8_t, globals::IO.m_size> IO       {};
        std::array<uint8_t, globals::HRAM.m_size> HRAM   {};
    };
}
