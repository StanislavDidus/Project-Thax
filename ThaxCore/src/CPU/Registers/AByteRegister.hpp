#pragma once
#include <cstdint>

namespace dmg
{
    class AByteRegister
    {
    public:
        AByteRegister() = default;
        virtual ~AByteRegister() = 0;

        [[nodiscard]] virtual uint8_t Read() const = 0;
        virtual void Write(uint8_t data) = 0;
    };

    inline AByteRegister::~AByteRegister()
    {
    }
}
