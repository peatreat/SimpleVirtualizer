#pragma once
#include "instruction.h"
#include "single_op.h"

class SetzInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        SingleOpFlags flags = *(SingleOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        int operand_sz;
        auto operand = flags.get_operand_ptr(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip, operand_sz);

        *(uint8_t*)operand = vm_data->eflags.ZF;

        vm_data->registers.ip += instruction_size;
    }
};