#pragma once
#include <iostream>
#include <intrin.h>
#include <vector>

#include "data.h"

#include "push.h"
#include "pop.h"
#include "add.h"
#include "sub.h"
#include "mov.h"
#include "lea.h"
#include "call.h"
#include "ret.h"
#include "jmp.h"
#include "test.h"
#include "xor.h"
#include "je.h"
#include "jne.h"
#include "jae.h"
#include "int3.h"
#include "cmp.h"
#include "stos.h"
#include "jle.h"
#include "movsx.h"
#include "inc.h"
#include "jl.h"
#include "dec.h"
#include "movzx.h"
#include "and.h"
#include "shr.h"
#include "neg.h"
#include "imul.h"
#include "cdqe.h"
#include "rol.h"
#include "ror.h"
#include "jnle.h"
#include "setnz.h"
#include "jnbe.h"
#include "cmovb.h"
#include "cmovz.h"
#include "cmovs.h"
#include "jbe.h"
#include "jnl.h"
#include "shl.h"
#include "not.h"
#include "or.h"
#include "jb.h"
#include "cmpxchg.h"
#include "movs.h"
#include "xchg.h"
#include "setz.h"
#include "cmovnz.h"
#include "cpuid.h"
#include "xgetbv.h"

#define InstructionHandler(type, obj_type, vm_data) \
case type: { \
    obj_type instruction; \
    instruction.execute(&vm_data); \
    break; \
} \

#define JccInstructionHandler(type, obj_type, vm_data) \
case type: { \
    obj_type instruction(&vm_data); \
    instruction.execute(&vm_data); \
    break; \
} \

const int STACK_SIZE = sizeof(uintptr_t) * 1000;

class VirtualMachine {
    VmData vm_data;
public:
	VirtualMachine(uintptr_t assembly_start, uintptr_t assembly_end) {
        vm_data.stack_base = (uintptr_t)_malloca(STACK_SIZE) + STACK_SIZE;
        vm_data.registers.set_register(Register::RSP, vm_data.stack_base);

        auto eflags = __readeflags();
        vm_data.eflags = *reinterpret_cast<EFlags*>(&eflags);

        vm_data.assembly_start = assembly_start;
        vm_data.assembly_end = assembly_end;
	}

    VmData* data() {
        return &this->vm_data;
    }

	uintptr_t execute(uintptr_t ip, uintptr_t rcx, uintptr_t rdx, uintptr_t r8, uintptr_t r9) {
        vm_data.registers.ip = (uint8_t*)ip;

        vm_data.registers.set_register(RCX, rcx);
        vm_data.registers.set_register(RDX, rdx);
        vm_data.registers.set_register(R8, r8);
        vm_data.registers.set_register(R9, r9);

        while (vm_data.registers.ip >= (uint8_t*)vm_data.assembly_start && vm_data.registers.ip < (uint8_t*)vm_data.assembly_end) {
            InstructionType type = *(InstructionType*)vm_data.registers.ip;

            switch (type) {
                case InstructionType::RET: {
                    RetInstruction instruction;

                    if (vm_data.registers.get_register<uintptr_t>(RSP) == vm_data.stack_base)
                        return vm_data.registers.get_register<uintptr_t>(RAX);

                    instruction.execute(&vm_data);
                    break;
                }
                InstructionHandler(InstructionType::PUSH, PushInstruction, vm_data);
                InstructionHandler(InstructionType::POP, PopInstruction, vm_data);
                InstructionHandler(InstructionType::ADD, AddInstruction, vm_data);
                InstructionHandler(InstructionType::SUB, SubInstruction, vm_data);
                InstructionHandler(InstructionType::MOV, MovInstruction, vm_data);
                InstructionHandler(InstructionType::LEA, LeaInstruction, vm_data);
                InstructionHandler(InstructionType::CALL, CallInstruction, vm_data);
                InstructionHandler(InstructionType::JMP, JmpInstruction, vm_data);
                InstructionHandler(InstructionType::TEST, TestInstruction, vm_data);
                InstructionHandler(InstructionType::CMP, CmpInstruction, vm_data);
                InstructionHandler(InstructionType::XOR, XorInstruction, vm_data);
                JccInstructionHandler(InstructionType::JE, JeInstruction, vm_data);
                JccInstructionHandler(InstructionType::JNE, JneInstruction, vm_data);
                JccInstructionHandler(InstructionType::JAE, JaeInstruction, vm_data);
                JccInstructionHandler(InstructionType::JLE, JleInstruction, vm_data);
                JccInstructionHandler(InstructionType::JL, JlInstruction, vm_data);
                JccInstructionHandler(InstructionType::JNLE, JnleInstruction, vm_data);
                JccInstructionHandler(InstructionType::JNBE, JnbeInstruction, vm_data);
                JccInstructionHandler(InstructionType::JBE, JbeInstruction, vm_data);
                JccInstructionHandler(InstructionType::JNL, JnlInstruction, vm_data);
                JccInstructionHandler(InstructionType::JB, JbInstruction, vm_data);
                InstructionHandler(InstructionType::INT3, Int3Instruction, vm_data);
                InstructionHandler(InstructionType::STOS, StosInstruction, vm_data);
                InstructionHandler(InstructionType::MOVSX, MovsxInstruction, vm_data);
                InstructionHandler(InstructionType::INC, IncInstruction, vm_data);
                InstructionHandler(InstructionType::DEC, IncInstruction, vm_data);
                InstructionHandler(InstructionType::MOVZX, MovzxInstruction, vm_data);
                InstructionHandler(InstructionType::AND, AndInstruction, vm_data);
                InstructionHandler(InstructionType::SHR, ShrInstruction, vm_data);
                InstructionHandler(InstructionType::NEG, NegInstruction, vm_data);
                InstructionHandler(InstructionType::IMUL, ImulInstruction, vm_data);
                InstructionHandler(InstructionType::CDQE, CdqeInstruction, vm_data);
                InstructionHandler(InstructionType::ROL, RolInstruction, vm_data);
                InstructionHandler(InstructionType::ROR, RorInstruction, vm_data);
                InstructionHandler(InstructionType::SETNZ, SetnzInstruction, vm_data);
                InstructionHandler(InstructionType::CMOVB, CmovbInstruction, vm_data);
                InstructionHandler(InstructionType::CMOVZ, CmovzInstruction, vm_data);
                InstructionHandler(InstructionType::CMOVS, CmovsInstruction, vm_data);
                InstructionHandler(InstructionType::SHL, ShlInstruction, vm_data);
                InstructionHandler(InstructionType::NOT, NotInstruction, vm_data);
                InstructionHandler(InstructionType::OR, OrInstruction, vm_data);
                InstructionHandler(InstructionType::CMPXCHG, CmpxchgInstruction, vm_data);
                InstructionHandler(InstructionType::MOVS, MovsInstruction, vm_data);
                InstructionHandler(InstructionType::XCHG, XchgInstruction, vm_data);
                InstructionHandler(InstructionType::SETZ, SetzInstruction, vm_data);
                InstructionHandler(InstructionType::CMOVNZ, CmovnzInstruction, vm_data);
                InstructionHandler(InstructionType::CPUID, CpuidInstruction, vm_data);
                InstructionHandler(InstructionType::XGETBV, XgetbvInstruction, vm_data);
            default:
                std::cout << "unhandled instruction type: " << std::hex << type << std::endl;;
                return 0;
            }
        }

        return vm_data.registers.get_register<uintptr_t>(RAX);
	}
};