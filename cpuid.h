#pragma once
#include "instruction.h"
#include "pop.h"

class CpuidInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        auto vm_eax = vm_data->registers.get_register<uint32_t>(RAX);
        auto vm_ecx = vm_data->registers.get_register<uint32_t>(RCX);

        uint32_t ret_eax, ret_ebx, ret_ecx, ret_edx;

        __asm {
            mov eax, vm_eax
            mov ecx, vm_ecx
            cpuid
            mov ret_eax, eax
            mov ret_ebx, ebx
            mov ret_ecx, ecx
            mov ret_edx, edx
        }

        vm_data->registers.set_register(RAX, ret_eax);
        vm_data->registers.set_register(RBX, ret_ebx);
        vm_data->registers.set_register(RCX, ret_ecx);
        vm_data->registers.set_register(RDX, ret_edx);

        vm_data->registers.ip += OPCODE_SIZE;
    }
};