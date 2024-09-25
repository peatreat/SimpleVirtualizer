#pragma once
#include "instruction.h"

class XgetbvInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        auto vm_ecx = vm_data->registers.get_register<uint32_t>(RCX);
        auto vm_rdx = vm_data->registers.get_register<uintptr_t>(RDX);
        auto vm_rax = vm_data->registers.get_register<uintptr_t>(RAX);

        __asm {
            mov ecx, vm_ecx
            mov rdx, vm_rdx
            mov rax, vm_rax

            xgetbv

            mov vm_rdx, rdx
            mov vm_rax, rax
        }

        vm_data->registers.set_register(RDX, vm_rdx);
        vm_data->registers.set_register(RAX, vm_rax);

        vm_data->registers.ip += OPCODE_SIZE;
    }
};