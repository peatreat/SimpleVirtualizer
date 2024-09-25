#pragma once
#include "instruction.h"
#include "single_op.h"

class NotInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        SingleOpFlags flags = *(SingleOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        int operand_sz;
        auto operand = flags.get_operand_ptr(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip, operand_sz);

        do_not((uint8_t*)operand, &vm_data->eflags, operand_sz);

        vm_data->registers.ip += instruction_size;
    }

private:
    void do_not(uint8_t* dst, EFlags* eflags, int op_size) {
        switch (op_size) {
            case sizeof(uint64_t) : {
                *(uint64_t*)dst = ~*(uint64_t*)dst;
                break;
            }
            case sizeof(uint32_t) : {
                *(uint32_t*)dst = ~*(uint32_t*)dst;
                break;
            }
            case sizeof(uint16_t) : {
                *(uint16_t*)dst = ~*(uint16_t*)dst;
                break;
            }
            case sizeof(uint8_t) : {
                *dst = ~*dst;
                break;
            }
        }
    }
};