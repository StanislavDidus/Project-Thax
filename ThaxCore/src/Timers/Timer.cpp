#include "Timers/Timer.hpp"

#include "DMG/Utils/Log.hpp"

namespace dmg
{
    Timer::Timer(InterruptController& interrupt_controller)
        : m_interrupt_controller(interrupt_controller)
    {
    }

    void Timer::Update(const uint32_t cycles)
    {
        if (overflow)
        {
            overflow_delay += cycles;
            if (overflow_delay >= 4)
            {
                TIMA = TMA;
                m_interrupt_controller.SetIF(InterruptController::Timer);
                overflow = false;
                overflow_delay = 0;
            }
        }

        for (int i = 0; i < cycles; ++i)
        {
            DIV++;

            if (IsClockEnabled())
            {
                const uint8_t bit_selected = GetSelectedBit();
                const uint8_t new_bit_value = getBit(DIV, bit_selected);

                if (old_bit_value == 1 && new_bit_value == 0)
                {
                    IncrementTima();
                }

                old_bit_value = new_bit_value;
            }
        }
    }

    void Timer::WriteTAC(const uint8_t value)
    {
        // When the clock is disabled and the selected bit is set we send an interrupt
        // This should not happen on the Game Boy Color

        const bool old = IsClockEnabled();
        TAC = value;
        const bool now = IsClockEnabled();

        if (old && !now && GetSelectedBit())
        {
            m_interrupt_controller.SetIF(InterruptController::Timer);
        }
    }

    uint8_t Timer::GetSelectedBit() const
    {
        const uint8_t frequency = TAC & 0b11;
        uint8_t bit_selected = 0;
        switch (frequency)
        {
        case 0b00:
            bit_selected = 9;
            break;
        case 0b01:
            bit_selected = 3;
            break;
        case 0b10:
            bit_selected = 5;
            break;
        case 0b11:
            bit_selected = 7;
            break;
        default:
            DMG_ERROR("Program should never reach this point. Timer frequency selection.");
        }

        return bit_selected;
    }

    void Timer::IncrementTima()
    {
        ++TIMA;

        if (TIMA == 0)
        {
            overflow_delay = 0;
            overflow = true;
        }
    }
}
