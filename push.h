#pragma once
#include "instruction.h"
#include "single_op.h"

class PushInstruction: Instruction {
public:
    void push_ptr(VmData* vm_data, uintptr_t ptr) {
        auto sp = vm_data->registers.get_register<uintptr_t*>(Register::RSP);
        sp--;

        vm_data->registers.set_register(Register::RSP, sp);

        *sp = ptr;
    }

	void execute(VmData* vm_data) {
		SingleOpFlags flags = *(SingleOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        int operand_sz;
        auto operand = flags.get_operand(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip, operand_sz);

        push_ptr(vm_data, operand);

        vm_data->registers.ip += instruction_size;
	}
};