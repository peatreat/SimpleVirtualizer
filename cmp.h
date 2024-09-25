#pragma once
#include "instruction.h"
#include "two_op.h"

class CmpInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        TwoOpFlags flags = *(TwoOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        auto result = flags.get_operands(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip);

        cmp(result.op1, result.op2, &vm_data->eflags, result.operation_size);

        vm_data->registers.ip += instruction_size;
    }

private:
    void cmp(uint8_t* dst, uint8_t* src, EFlags* eflags, int op_size) {
        switch (op_size) {
        case sizeof(uint64_t): {
            uint64_t v1 = *(uint64_t*)dst;
            uint64_t v2 = *(uint64_t*)src;

            __asm {
                mov rax, v1
                cmp rax, v2
            }

            eflags->value = __readeflags();
            break;
        }
        case sizeof(uint32_t) : {
            uint32_t v1 = *(uint32_t*)dst;
            uint32_t v2 = *(uint32_t*)src;

            __asm {
                mov eax, v1
                cmp eax, v2
            }

            eflags->value = __readeflags();
            break;
        }
        case sizeof(uint16_t) : {
            uint16_t v1 = *(uint16_t*)dst;
            uint16_t v2 = *(uint16_t*)src;

            __asm {
                mov ax, v1
                cmp ax, v2
            }

            eflags->value = __readeflags();
            break;
        }
        case sizeof(uint8_t) : {
            uint8_t v1 = *dst;
            uint8_t v2 = *src;

            __asm {
                mov al, v1
                cmp al, v2
            }

            eflags->value = __readeflags();
            break;
        }
        }
    }
};