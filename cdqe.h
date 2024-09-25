#pragma once
#include "instruction.h"

class CdqeInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        uint64_t vm_eax = vm_data->registers.get_register<uint32_t>(RAX);

        __asm {
            mov eax, vm_eax
            cdqe
            mov vm_eax, rax
        }

        vm_data->registers.set_register(RAX, vm_eax);

        vm_data->registers.ip += OPCODE_SIZE;
    }
};