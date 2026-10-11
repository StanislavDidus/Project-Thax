#pragma once
#include <cstdint>
#include <iostream>

#include "Constants.hpp"
#include "InterruptController.hpp"
#include "Utils/Math.hpp"

namespace dmg
{
    class SerialPort
    {
    public:
        explicit SerialPort(InterruptController& interrupt_controller)
            : m_interrupt_controller(interrupt_controller)
        {

        }
        ~SerialPort() = default;

        void Update(const uint32_t cycles)
        {
            if (IsTransferEnabled())
            {
                m_serial_cycles += cycles;
                while (m_serial_cycles >= TRANSFER_FREQUENCY)
                {
                    // When the transfer just started and debug mode is enabled we can print put the contents of the Serial Buffer
                    if constexpr (DEBUG_MODE)
                    {
                        if (m_transfer_progress == 0)
                            std::cout << SB;
                    }

                    m_serial_cycles -= TRANSFER_FREQUENCY;
                    constexpr uint8_t income_bit = 0;
                    SB = SB << 1 | income_bit; // Shift out the first bit and add the income bit
                    m_transfer_progress++;

                    if (m_transfer_progress >= 8)
                    {
                        EndTransfer();
                        m_transfer_progress = 0;
                        m_interrupt_controller.SetIF(InterruptController::Serial);
                    }
                }
            }
        }

        void WriteSB(const uint8_t value) { SB = value; }
        [[nodiscard]] uint8_t ReadSB() const { return SB; }

        void WriteSC(const uint8_t value) { SC = value; }
        [[nodiscard]] uint8_t ReadSC() const { return SC; }

    private:
        [[nodiscard]] bool IsTransferEnabled() const { return getBit(SC, 7); }
        void EndTransfer() { setBit(SC,7, 0); }

        static constexpr uint32_t TRANSFER_FREQUENCY = 512;

        uint8_t m_transfer_progress = 0;
        uint32_t m_serial_cycles = 0;

        uint8_t SB = 0;
        uint8_t SC = 0;

        InterruptController& m_interrupt_controller;
    };
}
