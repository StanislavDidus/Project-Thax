#pragma once
#include <array>

#include "Bus.hpp"
#include "Cartridge.hpp"
#include "Constants.hpp"
#include "Timers/Timer.hpp"
#include "Joypad.hpp"
#include "SerialPort.hpp"

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
            Joypad& joypad
            )
            : m_cartridge(cartridge)
            , m_interrupt_controller(interrupt_controller)
            , m_timer(timer)
            , m_serial_port(serial_port)
            , m_joypad(joypad)
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
            if (address::UNUSABLE.Contains(address))
            {
                return 0xFF;
            }
            if (address::IO.Contains(address))
            {
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
                    return IO[address - address::IO.m_start];
                }
            }
            if (address::HRAM.Contains(address))
            {
                return HRAM[address - address::HRAM.m_start];
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

            if (address::ROM_0.Contains(address) || address::ROM_N.Contains(address))
            {
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
            else if (address::UNUSABLE.Contains(address))
            {
                //
            }
            else if (address::IO.Contains(address))
            {
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
                    IO[address - address::IO.m_start] = value;
                    break;
                }
            }
            else if (address::HRAM.Contains(address))
            {
                HRAM[address - address::HRAM.m_start] = value;
            }
            else if (address::IE.Contains(address))
            {
                m_interrupt_controller.WriteIF(value);
            }
        }
    private:
        Cartridge& m_cartridge;
        InterruptController& m_interrupt_controller;
        Timer& m_timer;
        SerialPort& m_serial_port;
        Joypad& m_joypad;

        std::array<uint8_t, address::VRAM.m_size> VRAM   {};
        std::array<uint8_t, address::RAM_N.m_size> RAM_N {};
        std::array<uint8_t, address::WRAM.m_size> WRAM   {};
        std::array<uint8_t, address::OAM.m_size> OAM     {};
        std::array<uint8_t, address::IO.m_size> IO       {};
        std::array<uint8_t, address::HRAM.m_size> HRAM   {};
    };
}
