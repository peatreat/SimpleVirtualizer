#pragma once
#include "instruction.h"
#include "two_op.h"

class ImulInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        TwoOpFlags flags = *(TwoOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        auto result = flags.get_operands(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip);

        imul(result.op1, result.op2, &vm_data->eflags, result.dst_sz, result.src_is_imm);

        vm_data->registers.ip += instruction_size;
    }

private:
    void imul(uint8_t* dst, uint8_t* src, EFlags* eflags, int dst_sz, bool src_is_imm) {
        switch (dst_sz) {
            case sizeof(uint64_t) : {
                uint64_t v1 = *(uint64_t*)dst;
                uint64_t v2 = src_is_imm ? *(int*)src : *(uint64_t*)src;

                __asm {
                    mov rax, v1
                    imul rax, v2
                    mov v1, rax
                }

                eflags->value = __readeflags();

                *(uint64_t*)dst = v1;
                break;
            }
            case sizeof(uint32_t) : {
                uint32_t v1 = *(uint32_t*)dst;
                uint32_t v2 = *(uint32_t*)src;

                __asm {
                    mov eax, v1
                    imul eax, v2
                    mov v1, eax
                }

                eflags->value = __readeflags();

                *(uint32_t*)dst = v1;
                break;
            }
            case sizeof(uint16_t) : {
                uint16_t v1 = *(uint16_t*)dst;
                uint16_t v2 = *(uint16_t*)src;

                __asm {
                    mov ax, v1
                    imul ax, v2
                    mov v1, ax
                }

                eflags->value = __readeflags();

                *(uint16_t*)dst = v1;
                break;
            }
        }
    }
};