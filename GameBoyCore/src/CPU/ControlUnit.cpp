#include "ControlUnit.hpp"

#include <filesystem>

#include "DMG/Utils/Log.hpp"
#include "CPU.hpp"
#include "Utils/Math.hpp"

dmg::ControlUnit::ControlUnit(CPU& cpu)
    : m_cpu(cpu)
{
}

uint32_t dmg::ControlUnit::Step()
{
    const uint8_t opcode = m_cpu.ReadMemory(m_cpu.PC++);

    switch (opcode)
    {
        // 0x0 Column
        case 0x00: return NOP();                                       // NOP
        case 0x10: return STOP();                                      // STOP
        case 0x20: return JR_NZ();                                     // JR NZ, s8
        case 0x30: return JR_NC();                                     // JR NC, s8
        case 0x40: return LD(m_cpu.B, m_cpu.B);                        // LD B, B
        case 0x50: return LD(m_cpu.D, m_cpu.B);                        // LD D, B
        case 0x60: return LD(m_cpu.H, m_cpu.B);                        // LD H, B
        case 0x70: return LD(Address(m_cpu.HL.ReadWord()), m_cpu.B);   // LD (HL), B
        case 0x80: return ADD(m_cpu.A, m_cpu.B);                       // ADD A, B
        case 0x90: return SUB(m_cpu.B);                                // SUB B
        case 0xA0: return AND(m_cpu.B);                                // AND B
        case 0xB0: return OR(m_cpu.B);                                 // OR B
        case 0xC0: return RET_NZ();                                    // RET NZ
        case 0xD0: return RET_NC();                                    // RET NC
        case 0xE0: return LD_internal_a8();                       // LD (a8), A
        case 0xF0: return LD_internal_a8_();                     // LD A, (a8)

        // 0x1 Column
        case 0x01: return LD(m_cpu.BC);                                // LD BC, d16
        case 0x11: return LD(m_cpu.DE);                                // LD DE, d16
        case 0x21: return LD(m_cpu.HL);                                // LD HL, d16
        case 0x31: return LD(m_cpu.SP);                                // LD SP, d16
        case 0x41: return LD(m_cpu.B, m_cpu.C);                        // LD B, C
        case 0x51: return LD(m_cpu.D, m_cpu.C);                        // LD D, C
        case 0x61: return LD(m_cpu.H, m_cpu.C);                        // LD H, C
        case 0x71: return LD(Address(m_cpu.HL), m_cpu.C);              // LD (HL), C
        case 0x81: return ADD(m_cpu.A, m_cpu.C);                       // ADD A, C
        case 0x91: return SUB(m_cpu.C);                                // SUB C
        case 0xA1: return AND(m_cpu.C);                                // AND C
        case 0xB1: return OR(m_cpu.C);                                 // OR C
        case 0xC1: return POP(m_cpu.BC);                               // POP BC
        case 0xD1: return POP(m_cpu.DE);                               // POP DE
        case 0xE1: return POP(m_cpu.HL);                               // POP HL
        case 0xF1: return POP(m_cpu.AF);                               // POP AF

        // 0x2 Column
        case 0x02: return LD(Address(m_cpu.BC), m_cpu.A);              // LD (BC), A
        case 0x12: return LD(Address(m_cpu.DE), m_cpu.A);              // LD (DE), A
        case 0x22: return LD(Address(m_cpu.HL++), m_cpu.A);            // LD (HL+), A
        case 0x32: return LD(Address(m_cpu.HL--), m_cpu.A);            // LD (HL-), A
        case 0x42: return LD(m_cpu.B, m_cpu.D);                        // LD B, D
        case 0x52: return LD(m_cpu.D, m_cpu.D);                        // LD D, D
        case 0x62: return LD(m_cpu.H, m_cpu.D);                        // LD H, D
        case 0x72: return LD(Address(m_cpu.HL), m_cpu.D);              // LD (HL), D
        case 0x82: return ADD(m_cpu.A, m_cpu.D);                       // ADD A, D
        case 0x92: return SUB(m_cpu.D);                                // SUB D
        case 0xA2: return AND(m_cpu.D);                                // AND D
        case 0xB2: return OR(m_cpu.D);                                 // OR D
        case 0xC2: return JP_NZ();                                     // JP NZ, a16
        case 0xD2: return JP_NC();                                     // JP NC, a16
        case 0xE2: return LD_internal_c();                 // LD (C), A
        case 0xF2: return LD_internal_c_();               // LD A, (C)

        // 0x3 Column
        case 0x03: return INC(m_cpu.BC);                               // INC BC
        case 0x13: return INC(m_cpu.DE);                               // INC DE
        case 0x23: return INC(m_cpu.HL);                               // INC HL
        case 0x33: return INC(m_cpu.SP);                               // INC SP
        case 0x43: return LD(m_cpu.B, m_cpu.E);                        // LD B, E
        case 0x53: return LD(m_cpu.D, m_cpu.E);                        // LD D, E
        case 0x63: return LD(m_cpu.H, m_cpu.E);                        // LD H, E
        case 0x73: return LD(Address(m_cpu.HL), m_cpu.E);              // LD (HL), E
        case 0x83: return ADD(m_cpu.A, m_cpu.E);                       // ADD A, E
        case 0x93: return SUB(m_cpu.E);                                // SUB E
        case 0xA3: return AND(m_cpu.E);                                // AND E
        case 0xB3: return OR(m_cpu.E);                                 // OR E
        case 0xC3: return JP();                                        // JP a16
        case 0xD3: DMG_WARN("Instruction 0xD3 is non-existent."); return 4;
        case 0xE3: DMG_WARN("Instruction 0xE3 is non-existent."); return 4;
        case 0xF3: return DI();                                        // DI

        // 0x4 Column
        case 0x04: return INC(m_cpu.B);                                // INC B
        case 0x14: return INC(m_cpu.D);                                // INC D
        case 0x24: return INC(m_cpu.H);                                // INC H
        case 0x34: return INC(Address(m_cpu.HL));                      // INC (HL)
        case 0x44: return LD(m_cpu.B, m_cpu.H);                        // LD B, H
        case 0x54: return LD(m_cpu.D, m_cpu.H);                        // LD D, H
        case 0x64: return LD(m_cpu.H, m_cpu.H);                        // LD H, H
        case 0x74: return LD(Address(m_cpu.HL), m_cpu.H);              // LD (HL), H
        case 0x84: return ADD(m_cpu.A, m_cpu.H);                       // ADD A, H
        case 0x94: return SUB(m_cpu.H);                                // SUB H
        case 0xA4: return AND(m_cpu.H);                                // AND H
        case 0xB4: return OR(m_cpu.H);                                 // OR H
        case 0xC4: return CALL_NZ();                                   // CALL NZ, a16
        case 0xD4: return CALL_NC();                                   // CALL NC, a16

        // 0x5 Column
        case 0x05: return DEC(m_cpu.B);                                // DEC B
        case 0x15: return DEC(m_cpu.D);                                // DEC D
        case 0x25: return DEC(m_cpu.H);                                // DEC H
        case 0x35: return DEC(Address(m_cpu.HL));                      // DEC (HL)
        case 0x45: return LD(m_cpu.B, m_cpu.L);                        // LD B, L
        case 0x55: return LD(m_cpu.D, m_cpu.L);                        // LD D, L
        case 0x65: return LD(m_cpu.H, m_cpu.L);                        // LD H, L
        case 0x75: return LD(Address(m_cpu.HL), m_cpu.L);              // LD (HL), L
        case 0x85: return ADD(m_cpu.A, m_cpu.L);                       // ADD A, L
        case 0x95: return SUB(m_cpu.L);                                // SUB L
        case 0xA5: return AND(m_cpu.L);                                // AND L
        case 0xB5: return OR(m_cpu.L);                                 // OR L
        case 0xC5: return PUSH(m_cpu.BC);                              // PUSH BC
        case 0xD5: return PUSH(m_cpu.DE);                              // PUSH DE
        case 0xE5: return PUSH(m_cpu.HL);                              // PUSH HL
        case 0xF5: return PUSH(m_cpu.AF);                              // PUSH AF

        // 0x6 Column
        case 0x06: return LD(m_cpu.B, ReadD8());                       // LD B, d8
        case 0x16: return LD(m_cpu.D, ReadD8());                       // LD D, d8
        case 0x26: return LD(m_cpu.H, ReadD8());                       // LD H, d8
        case 0x36: return LD(Address(m_cpu.HL), ReadD8());             // LD (HL), d8
        case 0x46: return LD(m_cpu.B, Address(m_cpu.HL));              // LD B, (HL)
        case 0x56: return LD(m_cpu.D, Address(m_cpu.HL));              // LD D, (HL)
        case 0x66: return LD(m_cpu.H, Address(m_cpu.HL));              // LD H, (HL)
        case 0x76: return HALT();                                      // HALT
        case 0x86: return ADD(m_cpu.A, Address(m_cpu.HL));             // ADD A, (HL)
        case 0x96: return SUB(Address(m_cpu.HL));                      // SUB (HL)
        case 0xA6: return AND(Address(m_cpu.HL));                      // AND (HL)
        case 0xB6: return OR(Address(m_cpu.HL));                       // OR (HL)
        case 0xC6: return ADD(m_cpu.A, ReadD8());                      // ADD A, d8
        case 0xD6: return SUB(ReadD8());                               // SUB d8
        case 0xE6: return AND(ReadD8());                               // AND d8
        case 0xF6: return OR(ReadD8());                                // OR d8

        // 0x7 Column
        case 0x07: return RLCA();                                      // RLCA
        case 0x17: return RLA();                                       // RLA
        case 0x27: return DAA();                                       // DAA
        case 0x37: return SCF();                                       // SCF
        case 0x47: return LD(m_cpu.B, m_cpu.A);                        // LD B, A
        case 0x57: return LD(m_cpu.D, m_cpu.A);                        // LD D, A
        case 0x67: return LD(m_cpu.H, m_cpu.A);                        // LD H, A
        case 0x77: return LD(Address(m_cpu.HL), m_cpu.A);              // LD (HL), A
        case 0x87: return ADD(m_cpu.A, m_cpu.A);                       // ADD A, A
        case 0x97: return SUB(m_cpu.A);                                // SUB A
        case 0xA7: return AND(m_cpu.A);                                // AND A
        case 0xB7: return OR(m_cpu.A);                                 // OR A
        case 0xC7: return RST(0x00);                                   // RST 0
        case 0xD7: return RST(0x10);                                   // RST 2
        case 0xE7: return RST(0x20);                                   // RST 4
        case 0xF7: return RST(0x30);                                   // RST 6

        // 0x8 Column
        case 0x08: return LD_SP();                                     // LD (a16), SP
        case 0x18: return JR();                                        // JR s8
        case 0x28: return JR_Z(ReadS8());                              // JR Z, s8
        case 0x38: return JR_C(ReadS8());                              // JR C, s8
        case 0x48: return LD(m_cpu.C, m_cpu.B);                        // LD C, B
        case 0x58: return LD(m_cpu.E, m_cpu.B);                        // LD E, B
        case 0x68: return LD(m_cpu.L, m_cpu.B);                        // LD L, B
        case 0x78: return LD(m_cpu.A, m_cpu.B);                        // LD A, B
        case 0x88: return ADC(m_cpu.B);                                // ADC A, B
        case 0x98: return SBC(m_cpu.B);                                // SBC A, B
        case 0xA8: return XOR(m_cpu.B);                                // XOR B
        case 0xB8: return CP(m_cpu.B);                                 // CP B
        case 0xC8: return RET_Z();                                     // RET Z
        case 0xD8: return RET_C();                                     // RET C
        case 0xE8: return ADD(m_cpu.SP, ReadS8());                     // ADD SP, s8
        case 0xF8: return LD_SP_S8(m_cpu.HL); // LD HL, SP+s8

        // 0x9 Column
        case 0x09: return ADD(m_cpu.HL, m_cpu.BC);                     // ADD HL, BC
        case 0x19: return ADD(m_cpu.HL, m_cpu.DE);                     // ADD HL, DE
        case 0x29: return ADD(m_cpu.HL, m_cpu.HL);                     // ADD HL, HL
        case 0x39: return ADD(m_cpu.HL, m_cpu.SP);                     // ADD HL, SP
        case 0x49: return LD(m_cpu.C, m_cpu.C);                        // LD C, C
        case 0x59: return LD(m_cpu.E, m_cpu.C);                        // LD E, C
        case 0x69: return LD(m_cpu.L, m_cpu.C);                        // LD L, C
        case 0x79: return LD(m_cpu.A, m_cpu.C);                        // LD A, C
        case 0x89: return ADC(m_cpu.C);                                // ADC A, C
        case 0x99: return SBC(m_cpu.C);                                // SBC A, C
        case 0xA9: return XOR(m_cpu.C);                                // XOR C
        case 0xB9: return CP(m_cpu.C);                                 // CP C
        case 0xC9: return RET();                                       // RET
        case 0xD9: return RETI();                                      // RETI
        case 0xE9: return JP(m_cpu.HL);                                // JP HL
        case 0xF9: return LD_SP(m_cpu.SP);           // LD SP, HL

        // 0xA Column
        case 0x0A: return LD(m_cpu.A, Address(m_cpu.BC));              // LD A, (BC)
        case 0x1A: return LD(m_cpu.A, Address(m_cpu.DE));              // LD A, (DE)
        case 0x2A: return LD(m_cpu.A, Address(m_cpu.HL++));            // LD A, (HL+)
        case 0x3A: return LD(m_cpu.A, Address(m_cpu.HL--));            // LD A, (HL-)
        case 0x4A: return LD(m_cpu.C, m_cpu.D);                        // LD C, D
        case 0x5A: return LD(m_cpu.E, m_cpu.D);                        // LD E, D
        case 0x6A: return LD(m_cpu.L, m_cpu.D);                        // LD L, D
        case 0x7A: return LD(m_cpu.A, m_cpu.D);                        // LD A, D
        case 0x8A: return ADC(m_cpu.D);                                // ADC A, D
        case 0x9A: return SBC(m_cpu.D);                                // SBC A, D
        case 0xAA: return XOR(m_cpu.D);                                // XOR D
        case 0xBA: return CP(m_cpu.D);                                 // CP D
        case 0xCA: return JP_Z();                                      // JP Z, a16
        case 0xDA: return JP_C();                                      // JP C, a16
        case 0xEA: return LD_a16();                                    // LD (a16), A
        case 0xFA: return LD_a16_();                                   // LD A, (a16)

        // 0xB Column
        case 0x0B: return DEC(m_cpu.BC);                               // DEC BC
        case 0x1B: return DEC(m_cpu.DE);                               // DEC DE
        case 0x2B: return DEC(m_cpu.HL);                               // DEC HL
        case 0x3B: return DEC(m_cpu.SP);                               // DEC SP
        case 0x4B: return LD(m_cpu.C, m_cpu.E);                        // LD C, E
        case 0x5B: return LD(m_cpu.E, m_cpu.E);                        // LD E, E
        case 0x6B: return LD(m_cpu.L, m_cpu.E);                        // LD L, E
        case 0x7B: return LD(m_cpu.A, m_cpu.E);                        // LD A, E
        case 0x8B: return ADC(m_cpu.E);                                // ADC A, E
        case 0x9B: return SBC(m_cpu.E);                                // SBC A, E
        case 0xAB: return XOR(m_cpu.E);                                // XOR E
        case 0xBB: return CP(m_cpu.E);                                 // CP E
        case 0xCB: DMG_WARN("CB table is not implemented yet."); return 4;
        case 0xDB: DMG_WARN("Instruction 0xDB is non-existent."); return 4;
        case 0xEB: DMG_WARN("Instruction 0xEB is non-existent."); return 4;
        case 0xFB: return EI();                                        // EI

        // 0xC Column
        case 0x0C: return INC(m_cpu.C);                                // INC C
        case 0x1C: return INC(m_cpu.E);                                // INC E
        case 0x2C: return INC(m_cpu.L);                                // INC L
        case 0x3C: return INC(m_cpu.A);                                // INC A
        case 0x4C: return LD(m_cpu.C, m_cpu.H);                        // LD C, H
        case 0x5C: return LD(m_cpu.E, m_cpu.H);                        // LD E, H
        case 0x6C: return LD(m_cpu.L, m_cpu.H);                        // LD L, H
        case 0x7C: return LD(m_cpu.A, m_cpu.H);                        // LD A, H
        case 0x8C: return ADC(m_cpu.H);                                // ADC A, H
        case 0x9C: return SBC(m_cpu.H);                                // SBC A, H
        case 0xAC: return XOR(m_cpu.H);                                // XOR H
        case 0xBC: return CP(m_cpu.H);                                 // CP H
        case 0xCC: return CALL_Z();                                    // CALL Z, a16
        case 0xDC: return CALL_C();                                    // CALL C, a16
        case 0xEC: DMG_WARN("Instruction 0xEC is non-existent."); return 4;
        case 0xFC: DMG_WARN("Instruction 0xFC is non-existent."); return 4;

        // 0xD Column
        case 0x0D: return DEC(m_cpu.C);                                // DEC C
        case 0x1D: return DEC(m_cpu.E);                                // DEC E
        case 0x2D: return DEC(m_cpu.L);                                // DEC L
        case 0x3D: return DEC(m_cpu.A);                                // DEC A
        case 0x4D: return LD(m_cpu.C, m_cpu.L);                        // LD C, L
        case 0x5D: return LD(m_cpu.E, m_cpu.L);                        // LD E, L
        case 0x6D: return LD(m_cpu.L, m_cpu.L);                        // LD L, L
        case 0x7D: return LD(m_cpu.A, m_cpu.L);                        // LD A, L
        case 0x8D: return ADC(m_cpu.L);                                // ADC A, L
        case 0x9D: return SBC(m_cpu.L);                                // SBC A, L
        case 0xAD: return XOR(m_cpu.L);                                // XOR L
        case 0xBD: return CP(m_cpu.L);                                 // CP L
        case 0xCD: return CALL();                                      // CALL a16
        case 0xDD: DMG_WARN("Instruction 0xDD is non-existent."); return 4;
        case 0xED: DMG_WARN("Instruction 0xED is non-existent."); return 4;
        case 0xFD: DMG_WARN("Instruction 0xFD is non-existent."); return 4;

        // 0xE Column
        case 0x0E: return LD(m_cpu.C, ReadD8());                       // LD C, d8
        case 0x1E: return LD(m_cpu.E, ReadD8());                       // LD E, d8
        case 0x2E: return LD(m_cpu.L, ReadD8());                       // LD L, d8
        case 0x3E: return LD(m_cpu.A, ReadD8());                       // LD A, d8
        case 0x4E: return LD(m_cpu.C, Address(m_cpu.HL));              // LD C, (HL)
        case 0x5E: return LD(m_cpu.E, Address(m_cpu.HL));              // LD E, (HL)
        case 0x6E: return LD(m_cpu.L, Address(m_cpu.HL));              // LD L, (HL)
        case 0x7E: return LD(m_cpu.A, Address(m_cpu.HL));              // LD A, (HL)
        case 0x8E: return ADC(Address(m_cpu.HL));                      // ADC A, (HL)
        case 0x9E: return SBC(Address(m_cpu.HL));                      // SBC A, (HL)
        case 0xAE: return XOR(Address(m_cpu.HL));                      // XOR (HL)
        case 0xBE: return CP(Address(m_cpu.HL));                       // CP (HL)
        case 0xCE: return ADC(ReadD8());                               // ADC A, d8
        case 0xDE: return SBC(ReadD8());                               // SBC A, d8
        case 0xEE: return XOR(ReadD8());                               // XOR d8
        case 0xFE: return CP(ReadD8());                                // CP d8

        // 0xF Column
        case 0x0F: return RRCA();                                      // RRCA
        case 0x1F: return RRA();                                       // RRA
        case 0x2F: return CPL();                                       // CPL
        case 0x3F: return CCF();                                       // CCF
        case 0x4F: return LD(m_cpu.C, m_cpu.A);                        // LD C, A
        case 0x5F: return LD(m_cpu.E, m_cpu.A);                        // LD E, A
        case 0x6F: return LD(m_cpu.L, m_cpu.A);                        // LD L, A
        case 0x7F: return LD(m_cpu.A, m_cpu.A);                        // LD A, A
        case 0x8F: return ADC(m_cpu.A);                                // ADC A, A
        case 0x9F: return SBC(m_cpu.A);                                // SBC A, A
        case 0xAF: return XOR(m_cpu.A);                                // XOR A
        case 0xBF: return CP(m_cpu.A);                                 // CP A
        case 0xCF: return RST(0x08);                                   // RST 1
        case 0xDF: return RST(0x18);                                   // RST 3
        case 0xEF: return RST(0x28);                                   // RST 5
        case 0xFF: return RST(0x38);                                   // RST 7

        default:
            DMG_WARN("There is no matching instruction for this opcode: %d", opcode);
            return 4;
    }
}


uint32_t dmg::ControlUnit::NOP()
{
    return 4;
}

uint32_t dmg::ControlUnit::STOP()
{
    DMG_WARN("STOP function is not yet implemented.");

    return 4;
}

uint32_t dmg::ControlUnit::DI()
{
    DMG_WARN("DI instruction function is not yet implemented.");

    return 4;
}

 uint32_t dmg::ControlUnit::HALT()
{
    DMG_WARN("HALT instruction function is not yet implemented.");

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
    DMG_WARN("EI function is not yet implemented.");

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
    int32_t result = dst.ReadWord() + s8;

    m_cpu.F.SetFlagZ(false);
    m_cpu.F.SetFlagN(false);
    m_cpu.F.SetFlagC(result > 0xFFFF);
    m_cpu.F.SetFlagH((dst.ReadWord() & 0xFF) + (s8 & 0xFF) > 0xFF);

    dst.WriteWord(static_cast<uint16_t>(result));

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
    uint16_t result = value + 1;

    m_cpu.F.SetFlagZ(result == 0);
    m_cpu.F.SetFlagN(false);
    m_cpu.F.SetFlagH((value & 0xF + 1) > 0xF);
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

    return 20;
}

uint32_t dmg::ControlUnit::RETI()
{
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
