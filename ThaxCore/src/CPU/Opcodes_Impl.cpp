#include  "ControlUnit.hpp"
#include "CPU.hpp"

namespace dmg
{

    uint32_t dmg::ControlUnit::NOP()
    {
        return 4;
    }

    uint32_t dmg::ControlUnit::STOP()
    {
        m_cpu.m_current_state = CPU::State::STOP;

        return 4;
    }

    uint32_t dmg::ControlUnit::DI()
    {
        m_cpu.IME = false;

        return 4;
    }

     uint32_t dmg::ControlUnit::HALT()
    {
        m_cpu.m_current_state = CPU::State::HALT;

        return 4;
    }

    uint32_t dmg::ControlUnit::RLCA()
    {
        uint8_t value = m_cpu.A.Read();

        uint8_t first_bit = getBit(value, 7);

        m_cpu.F.SetFlagZ(false);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC((first_bit == 1));

        value = value << 1;

        setBit(value, 0, first_bit);
        m_cpu.A.Write(value);

        return 4;
    }

    uint32_t dmg::ControlUnit::RLA()
    {
        uint8_t value = m_cpu.A.Read();

        uint8_t first_bit = getBit(value, 7);
        uint8_t old_carry_value = m_cpu.F.ReadFlagC();

        m_cpu.F.SetFlagZ(false);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC((first_bit == 1));

        value = value << 1;

        setBit(value, 0, old_carry_value);
        m_cpu.A.Write(value);

        return 4;
    }

    // Implementation was found on this website: https://blog.ollien.com/posts/gb-daa/
    uint32_t dmg::ControlUnit::DAA()
    {
        uint8_t offset = 0u;
        bool should_carry = false;

        const uint8_t a_value = m_cpu.A.Read();
        const bool half_carry = m_cpu.F.ReadFlagH();
        const bool carry = m_cpu.F.ReadFlagC();
        const bool subtract = m_cpu.F.ReadFlagN();

        if (!subtract && (a_value & 0xF) > 0x09 || half_carry)
        {
            offset |= 0x06;
        }
        if (!subtract && a_value > 0x99 || carry)
        {
            offset |= 0x60;
            should_carry = true;
        }

        uint8_t result = 0u;
        if (!subtract)
            result = a_value + offset;
        else
            result = a_value - offset;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(should_carry);

        m_cpu.A.Write(result);

        return 4;
    }

    uint32_t dmg::ControlUnit::SCF()
    {
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC(true);

        return 4;
    }

    uint32_t dmg::ControlUnit::EI()
    {
        m_cpu.IME = true;

        return 4;
    }

    uint32_t dmg::ControlUnit::RRCA()
    {
        uint8_t value = m_cpu.A.Read();

        uint8_t last_bit = getBit(value, 0);

        m_cpu.F.SetFlagZ(false);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC((last_bit == 1));

        value = value >> 1;

        setBit(value, 7, last_bit);
        m_cpu.A.Write(value);

        return 4;
    }

    uint32_t dmg::ControlUnit::RRA()
    {
        uint8_t value = m_cpu.A.Read();

        uint8_t last_bit = getBit(value, 0);
        uint8_t old_carry_value = m_cpu.F.ReadFlagC();

        m_cpu.F.SetFlagZ(false);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);
        m_cpu.F.SetFlagC((last_bit == 1));

        value = value >> 1;

        setBit(value, 7, old_carry_value);
        m_cpu.A.Write(value);

        return 4;
    }

    uint32_t dmg::ControlUnit::CPL()
    {
        m_cpu.A.Write(~m_cpu.A.Read());

        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagH(true);

        return 4;
    }

    uint32_t dmg::ControlUnit::CCF()
    {
        m_cpu.F.SetFlagC(!m_cpu.F.ReadFlagC());

        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(false);

        return 4;
    }

    uint32_t dmg::ControlUnit::JR_NZ()
    {
        int8_t s8 = ReadS8();
        if (m_cpu.F.ReadFlagZ() == 0)
        {
            m_cpu.PC.JumpRelative(s8);
            return 12;
        }

        return 8;
    }

    uint32_t dmg::ControlUnit::JR_NC()
    {
        int8_t s8 = ReadS8();
        if (m_cpu.F.ReadFlagC() == 0)
        {
            m_cpu.PC.JumpRelative(s8);

            return 12;
        }

        return 8;
    }

    uint32_t dmg::ControlUnit::JR()
    {
        m_cpu.PC.JumpRelative(ReadS8());

        return 12;
    }

    uint32_t dmg::ControlUnit::JR_Z(int8_t s8)
    {
        if (m_cpu.F.ReadFlagZ() == 1)
        {
            m_cpu.PC.JumpRelative(s8);

            return 12;
        }

        return 8;
    }

    uint32_t dmg::ControlUnit::JR_C(int8_t s8)
    {
        if (m_cpu.F.ReadFlagC() == 1)
        {
            m_cpu.PC.JumpRelative(s8);

            return 12;
        }

         return 8;
    }

    uint32_t dmg::ControlUnit::JP()
    {
        m_cpu.PC.WriteWord(ReadA16());

        return 16;
    }

    uint32_t dmg::ControlUnit::JP(ATwoByteRegister& reg)
    {
        m_cpu.PC.WriteWord(reg.ReadWord());

        return 4;
    }

    uint32_t dmg::ControlUnit::JP_NZ()
    {
        uint16_t a16 = ReadA16();
        if (m_cpu.F.ReadFlagZ() == 0)
        {
            m_cpu.PC.WriteWord(a16);

            return 16;
        }

        return 12;
    }

    uint32_t dmg::ControlUnit::JP_NC()
    {
        uint16_t a16 = ReadA16();
        if (m_cpu.F.ReadFlagC() == 0)
        {
            m_cpu.PC.WriteWord(a16);

            return 16;
        }

        return 12;
    }

    uint32_t dmg::ControlUnit::JP_Z()
    {
        uint16_t a16 = ReadA16();
        if (m_cpu.F.ReadFlagZ() == 1)
        {
            m_cpu.PC.WriteWord(a16);

            return 16;
        }

        return 12;
    }

    uint32_t dmg::ControlUnit::JP_C()
    {
        uint16_t a16 = ReadA16();
        if (m_cpu.F.ReadFlagC() == 1)
        {
            m_cpu.PC.WriteWord(a16);

            return 16;
        }

        return 12;
    }

    uint32_t dmg::ControlUnit::RST(uint8_t value)
    {
        m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadHigh());
        m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadLow());
        m_cpu.PC.WriteLow(value);
        m_cpu.PC.WriteHigh(0x00);

        return 16;
    }

    uint32_t dmg::ControlUnit::LD(AByteRegister& dst, AByteRegister& src)
    {
        dst.Write(src.Read());

        return 4;
    }

    uint32_t dmg::ControlUnit::LD(Address address, AByteRegister& src)
    {
        m_cpu.WriteMemory(address.m_address, src.Read());

        return 8;
    }

    uint32_t dmg::ControlUnit::LD(AByteRegister& reg, uint8_t d8)
    {
        reg.Write(d8);

        return 8;
    }

    uint32_t dmg::ControlUnit::LD(Address address, uint8_t d8)
    {
        m_cpu.WriteMemory(address.m_address, d8);

        return 12;
    }

    uint32_t dmg::ControlUnit::LD(AByteRegister& reg, Address address)
    {
        reg.Write(m_cpu.ReadMemory(address.m_address));

        return 8;
    }

    uint32_t dmg::ControlUnit::LD_SP_S8(ATwoByteRegister& dst)
    {
        int8_t s8 = ReadS8();
        uint16_t sp = m_cpu.SP.ReadWord();
        uint16_t result = static_cast<uint16_t>(sp + s8);

        m_cpu.F.SetFlagZ(false);
        m_cpu.F.SetFlagN(false);
         m_cpu.F.SetFlagH(((sp & 0xF) + (static_cast<uint8_t>(s8) & 0xF)) > 0xF);
        m_cpu.F.SetFlagC(((sp & 0xFF) + (static_cast<uint8_t>(s8) & 0xFF)) > 0xFF);

        dst.WriteWord(result);
        return 12;
    }

    uint32_t dmg::ControlUnit::LD_SP(ATwoByteRegister& dst)
    {
        dst.WriteWord(m_cpu.HL.ReadWord());

        return 8;
    }

    uint32_t dmg::ControlUnit::LD_SP()
    {
        uint16_t a16 = ReadA16();
        m_cpu.WriteMemory(a16, m_cpu.SP.ReadLow());
        m_cpu.WriteMemory(a16 + 1, m_cpu.SP.ReadHigh());

        return 20;
    }

    uint32_t dmg::ControlUnit::LD_internal_a8()
    {
        m_cpu.WriteMemory(make_uint16_t(0xFF, ReadA8()), m_cpu.A.Read());

        return 12;
    }

    uint32_t dmg::ControlUnit::LD_internal_c()
    {
        m_cpu.WriteMemory(make_uint16_t(0xFF, m_cpu.C.Read()), m_cpu.A.Read());

        return 8;
    }

    uint32_t dmg::ControlUnit::LD_internal_a8_()
    {
        m_cpu.A.Write(m_cpu.ReadMemory(make_uint16_t(0xFF, ReadA8())));

        return 12;
    }

    uint32_t dmg::ControlUnit::LD_internal_c_()
    {
        m_cpu.A.Write(m_cpu.ReadMemory(make_uint16_t(0xFF, m_cpu.C.Read())));

        return 8;
    }

    uint32_t dmg::ControlUnit::LD_a16()
    {
        m_cpu.WriteMemory(ReadA16(), m_cpu.A.Read());

        return 16;
    }

    uint32_t dmg::ControlUnit::LD_a16_()
    {
        m_cpu.A.Write(m_cpu.ReadMemory(ReadA16()));

        return 16;
    }

    uint32_t dmg::ControlUnit::LD(ATwoByteRegister& reg)
    {
        reg.WriteWord(ReadD16());

        return 12;
    }

    uint32_t dmg::ControlUnit::ADD(AByteRegister& dst, const AByteRegister& src)
    {
        uint16_t result = dst.Read() + src.Read();

        m_cpu.F.SetFlagZ((result & 0xFF) == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(result > 0xFF);
        m_cpu.F.SetFlagH((dst.Read() & 0xF) + (src.Read() & 0xF) > 0xF);

        dst.Write(static_cast<uint8_t>(result));

        return 4;
    }

    uint32_t dmg::ControlUnit::ADD(AByteRegister& dst, Address address)
    {
        uint16_t result = dst.Read() + m_cpu.ReadMemory(address.m_address);

        m_cpu.F.SetFlagZ((result & 0xFF) == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(result > 0xFF);
        m_cpu.F.SetFlagH((dst.Read() & 0xF) + (m_cpu.ReadMemory(address.m_address) & 0xF) > 0xF);

        dst.Write(static_cast<uint8_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::ADD(AByteRegister& dst, uint8_t d8)
    {
        uint16_t result = dst.Read() + d8;

        m_cpu.F.SetFlagZ((result & 0xFF) == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(result > 0xFF);
        m_cpu.F.SetFlagH((dst.Read() & 0xF) + (d8 & 0xF) > 0xF);

        dst.Write(static_cast<uint8_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::ADD(ATwoByteRegister& dst, int8_t s8)
    {
        const uint16_t sp = dst.ReadWord();
        const uint16_t result = static_cast<uint16_t>(sp + s8);

        m_cpu.F.SetFlagZ(false);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH(((sp & 0x0F) + (s8 & 0x0F)) > 0x0F);
        m_cpu.F.SetFlagC(((sp & 0xFF) + (s8 & 0xFF)) > 0xFF);

        dst.WriteWord(result);

        return 16;
    }

    uint32_t dmg::ControlUnit::ADD(ATwoByteRegister& dst, ATwoByteRegister& src)
    {
        uint32_t result = dst.ReadWord() + src.ReadWord();

        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH((dst.ReadWord() & 0xFFF) + (src.ReadWord() & 0xFFF) > 0xFFF);
        m_cpu.F.SetFlagC(result > 0xFFFF);

        dst.WriteWord(static_cast<uint16_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::ADC(AByteRegister& src)
    {
        uint8_t carry_flag = m_cpu.F.ReadFlagC();
        uint16_t result = m_cpu.A.Read() + src.Read() + carry_flag;

        m_cpu.F.SetFlagZ((result & 0xFF) == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(result > 0xFF);
        m_cpu.F.SetFlagH(((m_cpu.A.Read() & 0xF) + (src.Read() & 0xF) + carry_flag) > 0xF);

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 4;
    }

    uint32_t dmg::ControlUnit::ADC(Address address)
    {
        uint8_t carry_flag = m_cpu.F.ReadFlagC();
        uint8_t address_value = m_cpu.ReadMemory(address.m_address);
        uint16_t result = m_cpu.A.Read() + address_value + carry_flag;

        m_cpu.F.SetFlagZ((result & 0xFF) == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(result > 0xFF);
        m_cpu.F.SetFlagH(((m_cpu.A.Read() & 0xF) + (address_value & 0xF) + carry_flag) > 0xF);

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::ADC(uint8_t d8)
    {
        uint8_t carry_flag = m_cpu.F.ReadFlagC();
        uint16_t result = m_cpu.A.Read() + d8 + carry_flag;

        m_cpu.F.SetFlagZ((result & 0xFF) == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(result > 0xFF);
        m_cpu.F.SetFlagH(((m_cpu.A.Read() & 0xF) + (d8 & 0xF) + carry_flag) > 0xF);

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::SUB(AByteRegister& a)
    {
        int16_t result = static_cast<int16_t>(m_cpu.A.Read() - a.Read());

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(result < 0);
        m_cpu.F.SetFlagH((m_cpu.A.Read() & 0xF) < (a.Read() & 0xF));

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 4;
    }

    uint32_t dmg::ControlUnit::SUB(Address address)
    {
        uint8_t memory_value = m_cpu.ReadMemory(address.m_address);
        int16_t result = static_cast<int16_t>(m_cpu.A.Read() - memory_value);

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(result < 0);
        m_cpu.F.SetFlagH((m_cpu.A.Read() & 0xF) < (memory_value & 0xF));

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::SUB(uint8_t d8)
    {
        int16_t result = static_cast<int16_t>(m_cpu.A.Read() - d8);

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(result < 0);
        m_cpu.F.SetFlagH((m_cpu.A.Read() & 0xF) < (d8 & 0xF));

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::SBC(AByteRegister& reg)
    {
        uint8_t carry_flag = m_cpu.F.ReadFlagC();
        int16_t result = static_cast<int16_t>(m_cpu.A.Read() - reg.Read() - carry_flag);

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(result < 0);
        m_cpu.F.SetFlagH((m_cpu.A.Read() & 0xF) - (reg.Read() & 0xF) - carry_flag  < 0);

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 4;
    }

    uint32_t dmg::ControlUnit::SBC(Address address)
    {
        uint8_t carry_flag = m_cpu.F.ReadFlagC();
        uint8_t address_value = m_cpu.ReadMemory(address.m_address);
        int16_t result = static_cast<int16_t>(m_cpu.A.Read() - address_value - carry_flag);

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(result < 0);
        m_cpu.F.SetFlagH((m_cpu.A.Read() & 0xF) - (address_value & 0xF) - carry_flag  < 0);

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::SBC(uint8_t d8)
    {
        uint8_t carry_flag = m_cpu.F.ReadFlagC();
        int16_t result = static_cast<int16_t>(m_cpu.A.Read() - d8 - carry_flag);

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(result < 0);
        m_cpu.F.SetFlagH((m_cpu.A.Read() & 0xF) - (d8 & 0xF) - carry_flag  < 0);

        m_cpu.A.Write(static_cast<uint8_t>(result));

        return 8;
    }

    uint32_t dmg::ControlUnit::AND(AByteRegister& a)
    {
        uint8_t result = m_cpu.A.Read() & a.Read();

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(true);

        m_cpu.A.Write(result);

        return 4;
    }

    uint32_t dmg::ControlUnit::AND(Address address)
    {
        uint8_t address_value = m_cpu.ReadMemory(address.m_address);
        uint8_t result = m_cpu.A.Read() & address_value;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(true);

        m_cpu.A.Write(result);

        return 8;
    }

    uint32_t dmg::ControlUnit::AND(uint8_t d8)
    {
        uint8_t result = m_cpu.A.Read() & d8;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(true);

        m_cpu.A.Write(result);

        return 8;
    }

    uint32_t dmg::ControlUnit::OR(const AByteRegister& a)
    {
        uint8_t result = m_cpu.A.Read() | a.Read();

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(false);

        m_cpu.A.Write(result);

        return 4;
    }

    uint32_t dmg::ControlUnit::OR(Address address)
    {
        uint8_t address_value = m_cpu.ReadMemory(address.m_address);
        uint8_t result = m_cpu.A.Read() | address_value;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(false);

        m_cpu.A.Write(result);

        return 8;
    }

    uint32_t dmg::ControlUnit::OR(uint8_t d8)
    {
        uint8_t result = m_cpu.A.Read() | d8;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(false);

        m_cpu.A.Write(result);

        return 8;
    }

    uint32_t dmg::ControlUnit::XOR(const AByteRegister& reg)
    {
        uint8_t result = m_cpu.A.Read() ^ reg.Read();

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(false);

        m_cpu.A.Write(result);

        return 4;
    }

    uint32_t dmg::ControlUnit::XOR(Address address)
    {
        uint8_t result = m_cpu.A.Read() ^ m_cpu.ReadMemory(address.m_address);

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(false);

        m_cpu.A.Write(result);

        return 8;
    }

    uint32_t dmg::ControlUnit::XOR(uint8_t d8)
    {
        uint8_t result = m_cpu.A.Read() ^ d8;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagC(false);
        m_cpu.F.SetFlagH(false);

        m_cpu.A.Write(result);

        return 8;
    }

    uint32_t dmg::ControlUnit::CP(const AByteRegister& reg)
    {
        uint8_t a_value = m_cpu.A.Read();
        uint8_t reg_value = reg.Read();

        m_cpu.F.SetFlagZ(a_value == reg_value);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(a_value < reg_value);
        m_cpu.F.SetFlagH((a_value & 0xF) < (reg_value & 0xF));

        return 4;
    }

    uint32_t dmg::ControlUnit::CP(Address address)
    {
        const uint8_t a_value = m_cpu.A.Read();
        const uint8_t reg_value = m_cpu.ReadMemory(address.m_address);

        m_cpu.F.SetFlagZ(a_value == reg_value);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(a_value < reg_value);
        m_cpu.F.SetFlagH((a_value & 0xF) < (reg_value & 0xF));

        return 8;
    }

    uint32_t dmg::ControlUnit::CP(uint8_t d8)
    {
        uint8_t a_value = m_cpu.A.Read();

        m_cpu.F.SetFlagZ(a_value == d8);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagC(a_value < d8);
        m_cpu.F.SetFlagH((a_value & 0xF) < (d8 & 0xF));

        return 8;
    }

    uint32_t dmg::ControlUnit::INC(ATwoByteRegister& reg)
    {
        ++reg;

        return 8;
    }

    uint32_t dmg::ControlUnit::INC(AByteRegister& reg)
    {
        uint8_t result = reg.Read() + 1;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH((reg.Read() & 0xF) == 0xF);
        //m_cpu.F.set_c_flag(result == 0);

        reg.Write(result);

        return 4;
    }

    uint32_t dmg::ControlUnit::INC(Address address)
    {
        uint8_t value = m_cpu.ReadMemory(address.m_address);
        uint8_t result = value + 1;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(false);
        m_cpu.F.SetFlagH((value & 0xF) == 0xF);
        //m_cpu.F.set_c_flag(result == 0);

        m_cpu.WriteMemory(address.m_address, result);

        return 12;
    }

    uint32_t dmg::ControlUnit::DEC(ATwoByteRegister& reg)
    {
        uint16_t result = reg.ReadWord() - 1;
        reg.WriteWord(result);

        return 8;
    }

    uint32_t dmg::ControlUnit::DEC(AByteRegister& reg)
    {
        uint8_t value = reg.Read();
        uint8_t result = value - 1;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagH(((value & 0xF) == 0));
        //m_cpu.F.set_c_flag(result == 0);

        reg.Write(result);

        return 4;
    }

    uint32_t dmg::ControlUnit::DEC(Address address)
    {
        uint8_t value = m_cpu.ReadMemory(address.m_address);
        uint8_t result = value - 1;

        m_cpu.F.SetFlagZ(result == 0);
        m_cpu.F.SetFlagN(true);
        m_cpu.F.SetFlagH(((value & 0xF) == 0));

        m_cpu.WriteMemory(address.m_address, result);

        return 12;
    }

    uint32_t dmg::ControlUnit::CALL_NZ()
    {
        uint16_t a16 = ReadA16();
        if (m_cpu.F.ReadFlagZ() == 0)
        {
            m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadHigh());
            m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadLow());
            m_cpu.PC.WriteWord(a16);

            return 24;
        }

        return 12;
    }

    uint32_t dmg::ControlUnit::CALL_NC()
    {
        uint16_t a16 = ReadA16();
        if (m_cpu.F.ReadFlagC() == 0)
        {
            m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadHigh());
            m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadLow());
            m_cpu.PC.WriteWord(a16);

            return 24;
        }

        return 12;
    }

    uint32_t dmg::ControlUnit::CALL_Z()
    {
        uint16_t a16 = ReadA16();
        if (m_cpu.F.ReadFlagZ() == 1)
        {
            m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadHigh());
            m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadLow());
            m_cpu.PC.WriteWord(a16);

            return 24;
        }

        return 12;
    }

    uint32_t dmg::ControlUnit::CALL_C()
    {
        uint16_t a16 = ReadA16();
        if (m_cpu.F.ReadFlagC() == 1)
        {
            m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadHigh());
            m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadLow());
            m_cpu.PC.WriteWord(a16);

            return 24;
        }

        return 12;
    }

    uint32_t dmg::ControlUnit::CALL()
    {
        uint16_t a16 = ReadA16();

        m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadHigh());
        m_cpu.WriteMemory(--m_cpu.SP, m_cpu.PC.ReadLow());
        m_cpu.PC.WriteWord(a16);

        return 24;
    }

    uint32_t dmg::ControlUnit::RET_NZ()
    {
        if (m_cpu.F.ReadFlagZ() == 0)
        {
            const uint8_t low = m_cpu.ReadMemory(m_cpu.SP++);
            const uint8_t high = m_cpu.ReadMemory(m_cpu.SP++);

            m_cpu.PC.WriteWord(high << 8 | low);

            return 20;
        }

        return 8;
    }

    uint32_t dmg::ControlUnit::RET_NC()
    {
        if (m_cpu.F.ReadFlagC() == 0)
        {
            const uint8_t low = m_cpu.ReadMemory(m_cpu.SP++);
            const uint8_t high = m_cpu.ReadMemory(m_cpu.SP++);

            m_cpu.PC.WriteWord(high << 8 | low);

            return 20;
        }

        return 8;
    }

    uint32_t dmg::ControlUnit::RET_Z()
    {
        if (m_cpu.F.ReadFlagZ() == 1)
        {
            const uint8_t low = m_cpu.ReadMemory(m_cpu.SP++);
            const uint8_t high = m_cpu.ReadMemory(m_cpu.SP++);

            m_cpu.PC.WriteWord(high << 8 | low);

            return 20;
        }

        return 8;
    }

    uint32_t dmg::ControlUnit::RET_C()
    {
        if (m_cpu.F.ReadFlagC() == 1)
        {
            const uint8_t low = m_cpu.ReadMemory(m_cpu.SP++);
            const uint8_t high = m_cpu.ReadMemory(m_cpu.SP++);

            m_cpu.PC.WriteWord(high << 8 | low);

            return 20;
        }

        return 8;
    }

    uint32_t dmg::ControlUnit::RET()
    {
        const uint8_t low = m_cpu.ReadMemory(m_cpu.SP++);
        const uint8_t high = m_cpu.ReadMemory(m_cpu.SP++);

        m_cpu.PC.WriteWord(high << 8 | low);

        return 16;
    }

    uint32_t dmg::ControlUnit::RETI()
    {
        m_cpu.IME = true;

        const uint8_t low = m_cpu.ReadMemory(m_cpu.SP++);
        const uint8_t high = m_cpu.ReadMemory(m_cpu.SP++);

        m_cpu.PC.WriteWord(high << 8 | low);

        return 16;
    }

    uint32_t dmg::ControlUnit::POP(ATwoByteRegister& reg)
    {
        reg.WriteLow(m_cpu.ReadMemory(m_cpu.SP++));
        reg.WriteHigh(m_cpu.ReadMemory(m_cpu.SP++));

        return 12;
    }

    uint32_t dmg::ControlUnit::PUSH(const ATwoByteRegister& reg)
    {
        m_cpu.WriteMemory(--m_cpu.SP, reg.ReadHigh());
        m_cpu.WriteMemory(--m_cpu.SP, reg.ReadLow());

        return 16;
    }

    uint8_t dmg::ControlUnit::ReadA8()
    {
        return m_cpu.Fetch();
    }

    uint16_t dmg::ControlUnit::ReadD16()
    {
        uint8_t lsb = m_cpu.Fetch();
        uint8_t msb = m_cpu.Fetch();
        return (msb << 8) | lsb;
    }

    int8_t dmg::ControlUnit::ReadS8()
    {
        return static_cast<int8_t>(m_cpu.Fetch());
    }

    uint16_t dmg::ControlUnit::ReadA16()
    {
        uint8_t lsb = m_cpu.Fetch();
        uint8_t msb = m_cpu.Fetch();
        return static_cast<uint16_t>(msb << 8 | lsb);
    }

    uint8_t dmg::ControlUnit::ReadD8()
    {
        return m_cpu.Fetch();
    }
}