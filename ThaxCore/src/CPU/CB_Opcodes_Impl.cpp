#include "ControlUnit.hpp"
#include "CPU/CPU.hpp"

namespace dmg
{
    uint32_t dmg::ControlUnit::RLC(AByteRegister& reg)
    {
        uint8_t value = reg.Read();

        const uint8_t first_bit = getBit(value, 7);
        value = value << 1;
        setBit(value, 0, first_bit);

        m_cpu.F.SetFlagZ(value == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(first_bit);

        reg.Write(value);

        return 8;
    }

    uint32_t dmg::ControlUnit::RL(AByteRegister& reg)
    {
        uint8_t value = reg.Read();

        const uint8_t first_bit = getBit(value, 7);
        value = value << 1;
        setBit(value, 0, m_cpu.F.ReadFlagC());

        m_cpu.F.SetFlagZ(value == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(first_bit);

        reg.Write(value);

        return 8;
    }

    uint32_t dmg::ControlUnit::SLA(AByteRegister& reg)
    {
        uint8_t value = reg.Read();

        const uint8_t first_bit = getBit(value, 7);
        value = value << 1;
        setBit(value, 0, 0);

        m_cpu.F.SetFlagZ(value == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(first_bit);

        reg.Write(value);

        return 8;
    }

    uint32_t ControlUnit::SWAP(AByteRegister& reg)
    {
        uint8_t value = reg.Read();

        uint8_t low = value & 0xF;
        uint8_t high = value & 0xF0;

        uint8_t result = (low << 4) | (high >> 4);

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(false);

        reg.Write(result);

        return 8;
    }

    uint32_t ControlUnit::BIT(const uint8_t bit, const AByteRegister& reg)
    {
        const uint8_t value = reg.Read();

        m_cpu.F.SetFlagZ(getBit(value, bit) == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(true);

        return 8;
    }

    uint32_t ControlUnit::RES(uint8_t bit, AByteRegister& reg)
    {
        uint8_t value = reg.Read();

        setBit(value, bit, 0);

        reg.Write(value);

        return 8;
    }

    uint32_t ControlUnit::SET(uint8_t bit, AByteRegister& reg)
    {
        uint8_t value = reg.Read();

        setBit(value, bit, 1);

        reg.Write(value);
    }

    uint32_t ControlUnit::RLC(Address address)
    {
        uint8_t value = m_cpu.ReadMemory(address.m_address);

        const uint8_t first_bit = getBit(value, 7);
        value = value << 1;
        setBit(value, 0, first_bit);

        m_cpu.F.SetFlagZ(value == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(first_bit);

        m_cpu.WriteMemory(address.m_address, value);

        return 8;
    }

    uint32_t ControlUnit::RL(Address address)
    {
        uint8_t value = m_cpu.ReadMemory(address.m_address);

        const uint8_t first_bit = getBit(value, 7);
        value = value << 1;
        setBit(value, 0, m_cpu.F.ReadFlagC());

        m_cpu.F.SetFlagZ(value == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(first_bit);

        m_cpu.WriteMemory(address.m_address, value);

        return 8;
    }

    uint32_t ControlUnit::SLA(Address address)
    {
        uint8_t value = m_cpu.ReadMemory(address.m_address);

        const uint8_t first_bit = getBit(value, 7);
        value = value << 1;
        setBit(value, 0, 0);

        m_cpu.F.SetFlagZ(value == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(first_bit);

        m_cpu.WriteMemory(address.m_address, value);

        return 8;
    }

    uint32_t ControlUnit::SWAP(Address address)
    {
        uint8_t value = m_cpu.ReadMemory(address.m_address);

        uint8_t low = value & 0xF;
        uint8_t high = value & 0xF0;

        uint8_t result = (low << 4) | (high >> 4);

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(false);

        m_cpu.WriteMemory(address.m_address, value);

        return 8;
    }

    uint32_t ControlUnit::BIT(uint8_t bit, Address address)
    {
        const uint8_t value = m_cpu.ReadMemory(address.m_address);

        m_cpu.F.SetFlagZ(getBit(value, bit) == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(true);

        return 8;
    }

    uint32_t ControlUnit::RES(uint8_t bit, Address address)
    {
        uint8_t value = m_cpu.ReadMemory(address.m_address);

        setBit(value, bit, 0);

        m_cpu.WriteMemory(address.m_address, value);

        return 8;
    }

    uint32_t ControlUnit::SET(uint8_t bit, Address address)
    {
        uint8_t value = m_cpu.ReadMemory(address.m_address);

        setBit(value, bit, 1);

        m_cpu.WriteMemory(address.m_address, value);
    }
}
