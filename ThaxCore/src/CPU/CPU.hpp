#pragma once

#include <cstdint>

#include "ControlUnit.hpp"
#include "DMG/RegisterInfo.hpp"
#include "MemoryBus/Bus.hpp"
#include "Registers/ByteRegister.hpp"
#include "Registers/ByteRegisterPair.hpp"
#include "Registers/FlagRegister.hpp"
#include "Registers/TwoByteRegister.hpp"
#include "InterruptController.hpp"

namespace dmg
{
    class CPU
    {
    public:
        enum class State
        {
            NORMAL,
            STOP,
            HALT,
        };

        explicit CPU(Bus& bus, InterruptController& interrupt_controller);


        uint32_t Step();
        void Reset();

        std::optional<uint32_t> HandleInterrupts();

        void UnHalt();

        [[nodiscard]] bool IsHalted() const;
        [[nodiscard]] bool IsStopped() const;

        [[nodiscard]] bool IsIMESet() const;

        [[nodiscard]] RegisterInfo GetRegisterInfo() const;
        void SetRegisters(const RegisterInfo& register_info);
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
        InterruptController& m_interrupt_controller;
        Bus& m_bus;

        bool IME = false;

        State m_current_state = State::NORMAL;

        friend class ControlUnit;
    };
}
