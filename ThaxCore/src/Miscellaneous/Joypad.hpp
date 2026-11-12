#pragma once

#include "InterruptController.hpp"
#include "DMG/Input.hpp"

namespace dmg
{
    class Joypad
    {
    public:
        explicit Joypad(InterruptController& interrupt_controller);
        ~Joypad() = default;

        void KeyPressed(JoypadButton button);
        void KeyReleased(JoypadButton button);

        void Reset();

        [[nodiscard]] uint8_t Read();
        void Write(uint8_t value);
    private:
        uint8_t m_dpad = 0;
        uint8_t m_buttons = 0;
        uint8_t m_select = 0;

        void KeyChanged(JoypadButton button, bool pressed);

        InterruptController& m_interrupt_controller;
    };
}
