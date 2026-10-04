#pragma once

#include <cstdint>

#include "ControlUnit.hpp"
#include "DMG/RegisterInfo.hpp"
#include "MemoryBus/Bus.hpp"
#include "Registers/ByteRegister.hpp"
#include "Registers/ByteRegisterPair.hpp"
#include "Registers/FlagRegister.hpp"
#include "Registers/TwoByteRegister.hpp"
#include "Interrupt.hpp"

namespace dmg
{
    class CPU
    {
    public:
        explicit CPU(Bus& bus);

        void SetRegisters(const RegisterInfo& register_info);

        uint32_t Step();
        void Reset();

        std::optional<uint32_t> HandleInterrupts();
        void SendInterrupt(uint8_t interrupt);

        [[nodiscard]] RegisterInfo GetRegisterInfo() const;
    private:
        uint8_t Fetch();
        [[nodiscard]] uint8_t ReadMemory(uint16_t address) const;
        void WriteMemory(uint16_t address, uint8_t value);

        ByteRegister A, B, C, D, E, H, L;
        FlagRegister F;
        ByteRegisterPair AF, BC, DE, HL;

        TwoByteRegister PC {0x100};
        TwoByteRegister SP;

        ControlUnit m_control_unit {*this};
        Bus& m_bus;

        bool IME = false;
        Interrupt IE {m_bus, 0xFFFF, false};
        Interrupt IF {m_bus, 0xFF0F, true};


        friend class ControlUnit;
    };
}
