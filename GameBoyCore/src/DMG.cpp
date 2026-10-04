#include "DMG/DMG.hpp"

#include "CPU/CPU.hpp"
#include "DMG/Utils/Log.hpp"
#include "MemoryBus/StandardBus.hpp"
#include "MemoryBus/TestBus.hpp"
#include "Cartridge.hpp"

dmg::DMG::DMG(InitState init_state)
    : m_init_state(init_state)
{
    if (init_state == InitState::STANDARD)
    {
        m_cartridge = std::make_unique<Cartridge>();
        m_interrupt_controller = std::make_unique<InterruptController>();
        m_bus = std::make_unique<StandardBus>(*m_cartridge, *m_interrupt_controller);
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
    if (m_state == State::PLAY)
    {
        StartFrame();

        while (total_cycles < cycle_target)
        {
            total_cycles += m_cpu->HandleInterrupts().value_or(0);
            total_cycles += m_cpu->Step();

            // If something something
            // m_cpu->SendInterrupt(Interrupt::VBLANK);
        }

        EndFrame();
    }
    else if (m_state == State::PAUSE)
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
    total_cycles += m_cpu->HandleInterrupts().value_or(0);
    total_cycles += m_cpu->Step();
}

void dmg::DMG::StepFrame()
{
    while (total_cycles < cycle_target)
    {
        total_cycles += m_cpu->HandleInterrupts().value_or(0);
        total_cycles += m_cpu->Step();
    }
}

void dmg::DMG::StepCycles(uint32_t target_cycles)
{
    uint32_t current_cycles = 0;
    while (current_cycles < target_cycles)
    {
        // NOTE: Should I add interrupt handling here as well?!
        // This is only used for testing so I am not sure
        current_cycles += m_cpu->Step();
    }
}

void dmg::DMG::SetState(State state)
{
    m_state = state;

    // Do actions when entering certain states
    switch (m_state)
    {
    case State::NONE:
        break;
    case State::IDLE:
        m_cpu->Reset();
        break;
    case State::PLAY:
        break;
    case State::PAUSE:
        break;
    default:
        break;
    }
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

/*
uint8_t dmg::DMG::ReadFromMemory(uint16_t address)
{
    return m_memory_bus->Read(address);
}

void dmg::DMG::WriteToMemory(uint16_t address, uint8_t value)
{
    return m_memory_bus->Write(address, value);
}

uint8_t* dmg::DMG::GetMemoryPointer()
{
    return m_memory_bus->GetDataPointer();
}

uint32_t dmg::DMG::GetMemorySize()
{
    return m_memory_bus->GetMemorySize();
}

void dmg::DMG::ClearMemory()
{
    m_memory_bus->Clear();
}
*/

