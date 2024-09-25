#pragma once
#include "instruction.h"
#include "two_op.h"

class CmovzInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        TwoOpFlags flags = *(TwoOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        auto result = flags.get_operands(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip);

        if (vm_data->eflags.ZF == 1)
            cmovz(result.op1, result.op2, result.operation_size);

        vm_data->registers.ip += instruction_size;
    }

private:
    void cmovz(uint8_t* dst, uint8_t* src, int op_size) {
        switch (op_size) {
            case sizeof(uint64_t) : {
                *(uint64_t*)dst = *(uint64_t*)src;
                break;
            }
            case sizeof(uint32_t) : {
                *(uint32_t*)dst = *(uint32_t*)src;
                break;
            }
            case sizeof(uint16_t) : {
                *(uint16_t*)dst = *(uint16_t*)src;
                break;
            }
        }
    }
};