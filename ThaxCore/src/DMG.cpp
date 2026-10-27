#include "DMG/DMG.hpp"

#include "Precompiled.hpp"

#include "CPU/CPU.hpp"
#include "PPU/PPU.hpp"
#include "DMG/Utils/Log.hpp"
#include "MemoryBus/StandardBus.hpp"
#include "MemoryBus/TestBus.hpp"
#include "Miscellaneous/Cartridge.hpp"
#include "Miscellaneous/Timer.hpp"
#include "Miscellaneous/SerialPort.hpp"
#include "Miscellaneous/Joypad.hpp"

dmg::DMG::DMG(InitState init_state)
    : m_init_state(init_state)
{
    if (init_state == InitState::STANDARD)
    {
        m_cartridge = std::make_unique<Cartridge>();
        m_interrupt_controller = std::make_unique<InterruptController>();
        m_timer = std::make_unique<Timer>(*m_interrupt_controller);
        m_serial_port = std::make_unique<SerialPort>(*m_interrupt_controller);
        m_joypad = std::make_unique<Joypad>(*m_interrupt_controller);
        m_ppu = std::make_unique<PPU>(*m_interrupt_controller);
        m_bus = std::make_unique<StandardBus>(
            *m_cartridge,
            *m_interrupt_controller,
            *m_timer,
            *m_serial_port,
            *m_joypad,
            *m_ppu
            );
        m_cpu = std::make_unique<CPU>(*m_bus, *m_interrupt_controller);
    }
    else if (init_state == InitState::TEST)
    {
        m_interrupt_controller = std::make_unique<InterruptController>();
        m_bus = std::make_unique<TestBus>();
        m_cpu = std::make_unique<CPU>(*m_bus, *m_interrupt_controller);
    }

    DMG_INFO("Game Boy was initialized.");
}

dmg::DMG::~DMG() = default;

void dmg::DMG::LoadROM(const std::filesystem::path& path)
{
    if (m_cartridge)
        m_cartridge->Load(path);
    else
        DMG_ERROR("Trying to load ROM when cartridge is not existed.");
}

void dmg::DMG::StartFrame()
{
    cycle_target = T_CYCLES_PER_FRAME - cycle_accumulator;
    total_cycles = 0;
}

void dmg::DMG::Update()
{
    if (m_play_state == PlayState::PLAY)
    {
        StartFrame();

        while (total_cycles < cycle_target)
        {
            StepInstruction();
        }

        EndFrame();
    }
    else if (m_play_state == PlayState::PAUSE)
    {
        if (total_cycles >= cycle_target)
        {
            EndFrame();
            StartFrame();
        }
    }
}

void dmg::DMG::EndFrame()
{
    if (total_cycles > cycle_target)
    {
        cycle_accumulator = total_cycles - cycle_target;
    }
    else if (total_cycles == cycle_target)
    {
        cycle_accumulator = 0;
    }
}

void dmg::DMG::StepInstruction()
{
    if (m_cpu->IsStopped())
    {
        total_cycles += 4;
        return;
    };

    uint32_t cycles = 0;

    if (m_cpu->IsHalted() && m_interrupt_controller->GetPendingInterrupt())
    {
        m_cpu->UnHalt();
    }
    if (const auto irq = m_cpu->HandleInterrupts())
        cycles += irq.value_or(0);
    else if (m_cpu->IsHalted())
    {
        cycles = 4;
    }
    else
    {
        cycles = m_cpu->Step();
    }

    total_cycles += cycles;

    if (m_timer) m_timer->Update(cycles);
    if (m_serial_port) m_serial_port->Update(cycles);
    if (m_ppu) m_ppu->Update(cycles);
}

void dmg::DMG::StepFrame()
{
    while (total_cycles < cycle_target)
    {
        StepInstruction();
    }
}

void dmg::DMG::StepCycles(uint32_t target_cycles)
{
    uint32_t current_cycles = 0;
    while (current_cycles < target_cycles)
    {
        if (m_cpu->IsStopped() || m_cpu->IsHalted())
        {
            current_cycles += 4;
            continue;
        }

        // NOTE: Should I add interrupt handling here as well?!
        // This is only used for testing so I am not sure
        current_cycles += m_cpu->Step();
    }
}

void dmg::DMG::KeyPressed(JoypadButton button)
{
    if (m_joypad) m_joypad->KeyPressed(button);
}

void dmg::DMG::KeyReleased(JoypadButton button)
{
    if (m_joypad) m_joypad->KeyReleased(button);
}

void dmg::DMG::SetState(PlayState state)
{
    m_play_state = state;

    // Do actions when entering certain states
    switch (m_play_state)
    {
    case PlayState::NONE:
        break;
    case PlayState::IDLE:
        m_cpu->Reset();
        break;
    case PlayState::PLAY:
        break;
    case PlayState::PAUSE:
        break;
    default:
        break;
    }
}

const dmg::globals::LCD_BUFFER& dmg::DMG::getLCDBuffer() const
{
    if (m_ppu)
        return m_ppu->getLCDBuffer();

    throw std::runtime_error{"Trying to get LCD Buffer without an existing PPU."};
}

dmg::globals::TILE_DATA_BUFFER dmg::DMG::getTileDataBuffer() const
{
    if (m_ppu)
        return m_ppu->getTileDataBuffer();

    throw std::runtime_error{"Trying to get LCD Buffer without an existing PPU."};
}

void dmg::DMG::Reset()
{
    m_cpu->Reset();
}

void dmg::DMG::SetRegisters(const RegisterInfo& register_info)
{
    m_cpu->SetRegisters(register_info);
}

dmg::RegisterInfo dmg::DMG::GetRegisters() const
{
    return m_cpu->GetRegisterInfo();
}

void dmg::DMG::ClearMemory()
{
    m_bus->Clear();
}

uint8_t dmg::DMG::ReadMemory(const uint16_t address) const
{
    return m_bus->Read(address);
}

void dmg::DMG::WriteMemory(const uint16_t address, const uint8_t value)
{
    m_bus->Write(address, value);
}