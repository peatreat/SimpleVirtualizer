#pragma once
#include "instruction.h"
#include "two_op.h"

class MovInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        TwoOpFlags flags = *(TwoOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        if ((uintptr_t)(vm_data->registers.ip - vm_data->assembly_start) == 0x5ed4)
            printf("a");

        auto result = flags.get_operands(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip);

        mov(result.op1, result.op2, result.operation_size, flags.has_flag(TwoOpFlags::DST_IS_REGISTER));

        vm_data->registers.ip += instruction_size;
    }

private:
    void mov(uint8_t* dst, uint8_t* src, int op_size, bool dst_is_reg) {
        switch (op_size) {
        case sizeof(uint64_t) : {
            *(uint64_t*)dst = *(uint64_t*)src;
            break;
        }
        case sizeof(uint32_t) : {
            if (dst_is_reg)
                *(uint64_t*)dst = *(uint32_t*)src;
            else
                *(uint32_t*)dst = *(uint32_t*)src;
            break;
        }
        case sizeof(uint16_t) : {
            *(uint16_t*)dst = *(uint16_t*)src;
            break;
        }
        case sizeof(uint8_t) : {
            *(uint8_t*)dst = *(uint8_t*)src;
            break;
        }
        }
    }
};