#include "Joypad.hpp"

#include "DMG/Utils/Log.hpp"

namespace dmg
{
    void Joypad::KeyPressed(const JoypadButton button)
    {
        KeyChanged(button, true);
    }

    void Joypad::KeyReleased(const JoypadButton button)
    {
        KeyChanged(button, false);
    }

    uint8_t Joypad::Read() const
    {
        uint8_t low = 0x0F;
        if (!(m_select & 0x10)) low &= m_dpad;
        if (!(m_select & 0x20)) low &= m_buttons;
        uint8_t result =  0xC0 | m_select | low;
        //printf("P1 read: sel=%02X buttons=%02X dpad=%02X -> %02X\n", m_select, m_buttons, m_dpad, result);
        return result;
    }

    void Joypad::Write(const uint8_t value)
    {
        const uint8_t before = Read() & 0x0F;
        m_select = value & 0x30;
        const uint8_t after = Read() & 0x0F;
        if (before & ~after)
            m_interrupt_controller.SetIF(InterruptController::Joypad);
    }

    /*void Joypad::SetP1Bit(Buttons button, const uint8_t value)
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
    }*/

    void Joypad::KeyChanged(const JoypadButton button, const bool pressed)
    {
        const uint8_t before = Read() & 0x0F;

        uint8_t* group = nullptr;
        uint8_t mask = 0;
        switch (button)
        {
        case JoypadButton::DPAD_RIGHT: group = &m_dpad;    mask = 0x01; break;
        case JoypadButton::DPAD_LEFT:  group = &m_dpad;    mask = 0x02; break;
        case JoypadButton::DPAD_UP:    group = &m_dpad;    mask = 0x04; break;
        case JoypadButton::DPAD_DOWN:  group = &m_dpad;    mask = 0x08; break;
        case JoypadButton::A:          group = &m_buttons; mask = 0x01; break;
        case JoypadButton::B:          group = &m_buttons; mask = 0x02; break;
        case JoypadButton::SELECT:     group = &m_buttons; mask = 0x04; break;
        case JoypadButton::START:      group = &m_buttons; mask = 0x08; break;
        default: return;
        }

        if (pressed) *group &= ~mask;
        else *group |= mask;

        const uint8_t after = Read() & 0x0F;
        if (before & ~after)
            m_interrupt_controller.SetIF(InterruptController::Joypad);
    }
}
