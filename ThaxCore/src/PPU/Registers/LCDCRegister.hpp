#pragma once
#include <cstdint>

#include "InternalGlobals.hpp"
#include "CPU/Registers/AByteRegister.hpp"

namespace dmg
{
    class LCDCRegister : AByteRegister
    {
    public:
        LCDCRegister() = default;
        explicit LCDCRegister(const uint8_t data) : m_data(data) {}
        ~LCDCRegister() override = default;

        [[nodiscard]] uint8_t Read() const override;
        void Write(uint8_t data) override;

        [[nodiscard]] bool IsLCDEnabled() const;
        [[nodiscard]] uint16_t GetWindowTileMapArea() const;
        [[nodiscard]] bool isWindowEnabled() const;
        [[nodiscard]] uint16_t GetBGWindowTileDataArea() const;
        [[nodiscard]] uint16_t GetBGTileMapArea() const;
        [[nodiscard]] ObjectSize GetObjectSize() const;
        [[nodiscard]] bool AreObjectsEnabled() const;
        [[nodiscard]] bool IsBGWindowPriorityEnabled() const;
    private:
        uint8_t m_data = 0;
    };
}
