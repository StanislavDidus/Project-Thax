#pragma once

#include "Registers/AByteRegister.hpp"
#include "Registers/Address.hpp"
#include "Registers/ATwoByteRegister.hpp"

namespace dmg
{
    class CPU;

    class ControlUnit
    {
    public:
        explicit ControlUnit(CPU& cpu);

        uint32_t Step();
    private:
        uint32_t NOP();
        uint32_t STOP();
        uint32_t DI();
        uint32_t HALT();
        uint32_t RLCA();
        uint32_t RLA();
        uint32_t DAA();
        uint32_t SCF();
        uint32_t EI();
        uint32_t RRCA();
        uint32_t RRA();
        uint32_t CPL();
        uint32_t CCF();

        uint32_t JR_NZ();
        uint32_t JR_NC();
        uint32_t JR();
        uint32_t JR_Z(int8_t s8);
        uint32_t JR_C(int8_t s8);
        uint32_t JP();
        uint32_t JP(ATwoByteRegister& reg);
        uint32_t JP_NZ();
        uint32_t JP_NC();
        uint32_t JP_Z();
        uint32_t JP_C();
        uint32_t RST(uint8_t value);

        uint32_t LD(AByteRegister& dst, AByteRegister& src);
        uint32_t LD(Address address, AByteRegister& src);
        uint32_t LD(AByteRegister& reg, uint8_t d8);
        uint32_t LD(Address address, uint8_t d8);
        uint32_t LD(AByteRegister& reg, Address address);
        //uint32_t LD(ATwoByteRegister& dst, uint16_t src);
        uint32_t LD_SP_S8(ATwoByteRegister& dst);
        uint32_t LD_SP(ATwoByteRegister& dst);
        uint32_t LD_SP();

        uint32_t LD_internal_a8();
        uint32_t LD_internal_c();
        uint32_t LD_internal_a8_();
        uint32_t LD_internal_c_();
        uint32_t LD_a16();
        uint32_t LD_a16_();

        uint32_t LD(ATwoByteRegister& reg);

        uint32_t ADD(AByteRegister& dst, const AByteRegister& src);
        uint32_t ADD(AByteRegister& dst, Address address);
        uint32_t ADD(AByteRegister& dst, uint8_t d8);
        uint32_t ADD(ATwoByteRegister& dst, int8_t s8);
        uint32_t ADD(ATwoByteRegister& dst, ATwoByteRegister& src);
        uint32_t ADC(AByteRegister& src);
        uint32_t ADC(Address address);
        uint32_t ADC(uint8_t d8);

        uint32_t SUB(AByteRegister& a);
        uint32_t SUB(Address address);
        uint32_t SUB(uint8_t d8);
        uint32_t SBC(AByteRegister& reg);
        uint32_t SBC(Address address);
        uint32_t SBC(uint8_t d8);

        uint32_t AND(AByteRegister& a);
        uint32_t AND(Address address);
        uint32_t AND(uint8_t d8);
        uint32_t OR(const AByteRegister& a);
        uint32_t OR(Address address);
        uint32_t OR(uint8_t d8);
        uint32_t XOR(const AByteRegister& reg);
        uint32_t XOR(Address address);
        uint32_t XOR(uint8_t d8);
        uint32_t CP(const AByteRegister& reg);
        uint32_t CP(Address address);
        uint32_t CP(uint8_t d8);

        uint32_t INC(ATwoByteRegister& reg);
        uint32_t INC(AByteRegister& reg);
        uint32_t INC(Address address);
        uint32_t DEC(ATwoByteRegister& reg);
        uint32_t DEC(AByteRegister& reg);
        uint32_t DEC(Address address);

        uint32_t CALL_NZ();
        uint32_t CALL_NC();
        uint32_t CALL_Z();
        uint32_t CALL_C();
        uint32_t CALL();

        uint32_t RET_NZ();
        uint32_t RET_NC();
        uint32_t RET_Z();
        uint32_t RET_C();
        uint32_t RET();
        uint32_t RETI();

        uint32_t POP(ATwoByteRegister& reg);
        uint32_t PUSH(const ATwoByteRegister& reg);

        uint8_t ReadA8();
        uint16_t ReadD16();
        int8_t ReadS8();
        uint16_t ReadA16();
        uint8_t ReadD8();

        CPU& m_cpu;
    };
}
