#pragma once
#include "instruction.h"
#include "two_op.h"

class LeaInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        TwoOpFlags flags = *(TwoOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        auto result = flags.get_operands(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip);

        if (flags.has_flag(TwoOpFlags::SRC_IS_SIGNED_INT)) {
            int ip_offset = *(int*)result.op2;
            uintptr_t calculated_address = (uintptr_t)vm_data->registers.ip + instruction_size + ip_offset;

            *(uintptr_t*)result.op1 = calculated_address;
        }
        else {
            *(uintptr_t*)result.op1 = (uintptr_t)result.op2;
        }

        vm_data->registers.ip += instruction_size;
    }
};