#pragma once
#include "instruction.h"
#include "two_op.h"

class MovzxInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        TwoOpFlags flags = *(TwoOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        auto result = flags.get_operands(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip);

        movzx(result.op1, result.op2, result.operation_size, result.dst_sz);

        vm_data->registers.ip += instruction_size;
    }

private:
    void movzx(uint8_t* dst, uint8_t* src, int op_size, int dst_sz) {
        uint64_t new_dst;

        switch (op_size) {
            case sizeof(int) : {
                new_dst = *(int*)src;
                new_dst &= ~0xFFFFFFFF00000000;
                break;
            }
            case sizeof(short) : {
                new_dst = *(short*)src;
                new_dst &= ~0xFFFFFFFFFFFF0000;
                break;
            }
            case sizeof(int8_t) : {
                new_dst = *(int8_t*)src;
                new_dst &= ~0xFFFFFFFFFFFFFF00;
                break;
            }
        }

        switch (dst_sz) {
            case sizeof(uint64_t) :
                *(uint64_t*)dst = new_dst;
                break;
            case sizeof(int) :
                *(int*)dst = (int)new_dst;
                break;
            case sizeof(short) :
                *(short*)dst = (short)new_dst;
                break;
        }
    }
};