#pragma once

#include "InterruptController.hpp"
#include "DMG/Input.hpp"

namespace dmg
{
    class Joypad
    {
    public:
        explicit Joypad(InterruptController& interrupt_controller)
            : m_interrupt_controller(interrupt_controller)
        {}
        ~Joypad() = default;

        void KeyPressed(JoypadButton button);
        void KeyReleased(JoypadButton button);

        [[nodiscard]] uint8_t Read() const;
        void Write(uint8_t value);
    private:
        uint8_t m_dpad = 0x0F;
        uint8_t m_buttons = 0x0F;
        uint8_t m_select = 0x30;

        /*
        enum class Buttons : uint8_t
        {
            A_RIGHT        = 0,
            B_LEFT         = 1,
            SELECT_UP      = 2,
            START_DOWN     = 3,
            SELECT_DPAD    = 4,
            SELECT_BUTTONS = 5,
        };
        */

        //[[nodiscard]] bool IsButtonSelect() const;
        //[[nodiscard]] bool IsDPADSelect() const;
        //void SetP1Bit(Buttons button , uint8_t value);
        void KeyChanged(JoypadButton button, bool pressed);

        InterruptController& m_interrupt_controller;
    };
}
