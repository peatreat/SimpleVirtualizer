#pragma once
#include "instruction.h"
#include "single_op.h"

class PopInstruction : Instruction {
public:
    uintptr_t pop_ptr(VmData* vm_data) {
        auto sp = vm_data->registers.get_register<uintptr_t*>(Register::RSP);

        auto popped = *sp;

        sp++;

        vm_data->registers.set_register(Register::RSP, sp);

        return popped;
    }

    void execute(VmData* vm_data) {
        SingleOpFlags flags = *(SingleOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        auto popped = pop_ptr(vm_data);

        int operand_sz;
        *flags.get_operand_ptr(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip, operand_sz) = popped;

        vm_data->registers.ip += instruction_size;
    }
};