#pragma once
#include <cstdint>

#include "InternalGlobals.hpp"
#include "CPU/Registers/AByteRegister.hpp"

namespace dmg
{
    class STATRegister : AByteRegister
    {
    public:
        STATRegister() = default;
        explicit STATRegister(const uint8_t data) : m_data(data) {}
        ~STATRegister() override = default;

        [[nodiscard]] uint8_t Read() const override;
        void Write(uint8_t data) override;

        [[nodiscard]] bool IsLYCIntSelect() const;
        [[nodiscard]] bool IsMode2InSelect() const;
        [[nodiscard]] bool IsMode1InSelect() const;
        [[nodiscard]] bool IsMode0InSelect() const;
        [[nodiscard]] bool isLYCeqLC() const;
        [[nodiscard]] PPUMode getPPUMode() const;
    private:
        uint8_t m_data = 0;
    };
}
