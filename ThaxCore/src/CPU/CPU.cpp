#include "CPU.hpp"

#include <utility>

#include "DMG/Utils/Log.hpp"

dmg::CPU::CPU(Bus& bus, InterruptController& interrupt_controller)
    : AF{A, F}
    , BC{ B, C}
    , DE{ D, E}
    , HL{H, L}
    , m_interrupt_controller(interrupt_controller)
    , m_bus(bus)
{
}

void dmg::CPU::SetRegisters(const RegisterInfo& register_info)
{
    PC.WriteWord(register_info.PC);
    SP.WriteWord(register_info.SP);
    A.Write(register_info.A);
    B.Write(register_info.B);
    C.Write(register_info.C);
    D.Write(register_info.D);
    E.Write(register_info.E);
    F.Write(register_info.F);
    H.Write(register_info.H);
    L.Write(register_info.L);
}

uint32_t dmg::CPU::Step()
{
    return m_control_unit.Step();
}

void dmg::CPU::Reset()
{
    PC.WriteWord(0x100);
    SP.WriteWord(0);
    A.Write(0);
    F.Write(0);
    B.Write(0);
    C.Write(0);
    D.Write(0);
    E.Write(0);
    H.Write(0);
    L.Write(0);

    m_current_state = State::NORMAL;
}

std::optional<uint32_t> dmg::CPU::HandleInterrupts()
{
    if (IME)
    {
        const auto& result = m_interrupt_controller.GetSharedBit();
        if (result.has_value())
        {
            uint8_t value = result.value();

            IME = false;
            m_interrupt_controller.ResetIF(1 << value);

            WriteMemory(--SP, PC.ReadHigh());
            WriteMemory(--SP, PC.ReadLow());

            switch (value)
            {
            case 0: // VBLANK
                PC.WriteWord(0x40);
                break;
            case 1: // LCD
                PC.WriteWord(0x48);
                break;
            case 2: // Timer
                PC.WriteWord(0x50);
                break;
            case 3: // Serial
                PC.WriteWord(0x58);
                break;
            case 4: // Joypad
                PC.WriteWord(0x60);
                break;
            default:
                DMG_WARN("Interrupt returned a flag that is not supported: {}", value);
                break;
            }

            return 20;
        }
    }

    return std::nullopt;
}

void dmg::CPU::UnHalt()
{
    m_current_state = State::NORMAL;
}

bool dmg::CPU::IsHalted() const
{
    return m_current_state == State::HALT;
}

bool dmg::CPU::IsStopped() const
{
    return m_current_state == State::STOP;
}

bool dmg::CPU::IsIMESet() const
{
    return IME;
}

dmg::RegisterInfo dmg::CPU::GetRegisterInfo() const
{
    return RegisterInfo
    {
        .F = F.Read(),
        .A = A.Read(),
        .B = B.Read(),
        .C = C.Read(),
        .D = D.Read(),
        .E = E.Read(),
        .H = H.Read(),
        .L = L.Read(),
        .PC = PC.ReadWord(),
        .SP = SP.ReadWord(),
    };
}

uint8_t dmg::CPU::Fetch()
{
    return m_bus.Read(PC++);
}

uint8_t dmg::CPU::ReadMemory(uint16_t address) const
{
    return m_bus.Read(address);
}

void dmg::CPU::WriteMemory(uint16_t address, uint8_t value)
{
    m_bus.Write(address, value);
}
