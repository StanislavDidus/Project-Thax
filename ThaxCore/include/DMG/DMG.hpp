#pragma once

#include <filesystem>

#include "ExternalGlobals.hpp"
#include "Input.hpp"
#include "Info.hpp"

namespace dmg
{
    constexpr uint32_t T_CYCLES_PER_SECOND = 4'194'304;
    constexpr uint32_t M_CYCLES_PER_SECOND = 1'048'576;
    constexpr uint32_t T_CYCLES_PER_FRAME = 70'221;
    constexpr uint32_t M_CYCLES_PER_FRAME = 17'555;
    constexpr float FRAMES_PER_SECOND = 59.73f;

    class Cartridge;
    class CPU;
    class PPU;
    class Bus;
    class InterruptController;
    class Timer;
    class SerialPort;
    class Joypad;

    class DMG
    {
    public:
        enum class PlayState
        {
            NONE,
            IDLE,
            PLAY,
            PAUSE,
        };

        enum class InitState
        {
            STANDARD,
            TEST,
        };

        explicit DMG(InitState init_state = InitState::STANDARD);
        ~DMG();

        void LoadROM(const std::filesystem::path& path);

        void Update();

        void StepInstruction();
        void StepFrame();
        void StepCycles(uint32_t target_cycles);

        void KeyPressed(JoypadButton button);
        void KeyReleased(JoypadButton button);

        void SetState(PlayState state);

        // Setters and getters that are needed for objects outside of DMG class
        PlayState GetState() { return m_play_state; }

        [[nodiscard]] const globals::LCD_BUFFER& getLCDBuffer() const;
        [[nodiscard]] globals::TILE_DATA_BUFFER getTileDataBuffer() const;

        void Reset();

        void SetRegisters(const CPUInfo& register_info);
        [[nodiscard]] DMGInfo GetDMGInfo() const;

        void ClearMemory();
        [[nodiscard]] uint8_t ReadMemory(uint16_t address) const;
        void WriteMemory(uint16_t address, uint8_t value);
    private:
        void StartFrame();
        void EndFrame();

        std::unique_ptr<Cartridge> m_cartridge {};
        std::unique_ptr<Timer> m_timer {};
        std::unique_ptr<SerialPort> m_serial_port {};
        std::unique_ptr<Joypad> m_joypad {};
        std::unique_ptr<InterruptController> m_interrupt_controller {};
        std::unique_ptr<Bus> m_bus {};
        std::unique_ptr<CPU> m_cpu {};
        std::unique_ptr<PPU> m_ppu {};

        PlayState m_play_state = PlayState::NONE;
        InitState m_init_state = InitState::STANDARD;

        uint32_t cycle_accumulator = 0;
        uint32_t cycle_target = T_CYCLES_PER_FRAME;
        uint32_t total_cycles = 0;

        friend Bus;
    };
}
