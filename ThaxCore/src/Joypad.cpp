#include "Joypad.hpp"

#include "DMG/Utils/Log.hpp"

namespace dmg
{
    void Joypad::KeyPressed(const JoypadButton button)
    {
        KeyChanged(button, 0);
    }

    void Joypad::KeyReleased(const JoypadButton button)
    {
        KeyChanged(button, 1);
    }

    uint8_t Joypad::Read() const
    {
        return P1 == 0x30 ? P1 | 0x0F : P1;
    }

    void Joypad::Write(const uint8_t value)
    {
        P1 = value & 0xF0;
    }

    bool Joypad::IsButtonSelect() const
    {
        return getBit(P1, 5);
    }

    bool Joypad::IsDPADSelect() const
    {
        return getBit(P1, 4);
    }

    void Joypad::SetP1Bit(Buttons button, const uint8_t value)
    {
        const auto bit = static_cast<uint8_t>(button);

        const uint8_t old = getBit(P1, bit);
        setBit(P1, bit, value);
        const uint8_t now = getBit(P1, bit);

        // If button was not pressed and now is pressed - we send an interrupt
        if (old && !now && !value)
        {
            m_interrupt_controller.SetIF(InterruptController::Joypad);
        }
    }

    void Joypad::KeyChanged(const JoypadButton button, const uint8_t value)
    {
        if (IsButtonSelect())
        {
            switch (button)
            {
            case JoypadButton::A:
                setBit(P1, static_cast<uint8_t>(Buttons::A_RIGHT), value);
                break;
            case JoypadButton::B:
                setBit(P1, static_cast<uint8_t>(Buttons::B_LEFT), value);
                break;
            case JoypadButton::SELECT:
                setBit(P1, static_cast<uint8_t>(Buttons::SELECT_UP), value);
                break;
            case JoypadButton::START:
                setBit(P1, static_cast<uint8_t>(Buttons::START_DOWN), value);
                break;
            default:
                DMG_ERROR("Program should never reach this point.");
                break;
            }
        }

        if (IsDPADSelect())
        {
            switch (button)
            {
            case JoypadButton::DPAD_RIGHT:
                setBit(P1, static_cast<uint8_t>(Buttons::A_RIGHT), value);
                break;
            case JoypadButton::DPAD_LEFT:
                setBit(P1, static_cast<uint8_t>(Buttons::B_LEFT), value);
                break;
            case JoypadButton::DPAD_UP:
                setBit(P1, static_cast<uint8_t>(Buttons::SELECT_UP), value);
                break;
            case JoypadButton::DPAD_DOWN:
                setBit(P1, static_cast<uint8_t>(Buttons::START_DOWN), value);
                break;
            default:
                DMG_ERROR("Program should never reach this point.");
                break;
            }
        }
    }
}
