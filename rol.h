#pragma once
#include "instruction.h"
#include "two_op.h"

class RolInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        TwoOpFlags flags = *(TwoOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        auto result = flags.get_operands(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip);

        rol(result.op1, result.op2, &vm_data->eflags, result.dst_sz);

        vm_data->registers.ip += instruction_size;
    }

private:
    void rol(uint8_t* dst, uint8_t* src, EFlags* eflags, int dst_size) {
        switch (dst_size) {
            case sizeof(uint64_t) : {
                uint64_t v1 = *(uint64_t*)dst;
                uint8_t v2 = *src;

                __asm {
                    mov rax, v1
                    mov cl, v2
                    rol rax, cl
                    mov v1, rax
                }

                eflags->value = __readeflags();

                *(uint64_t*)dst = v1;
                break;
            }
            case sizeof(uint32_t) : {
                uint32_t v1 = *(uint32_t*)dst;
                uint8_t v2 = *src;

                __asm {
                    mov eax, v1
                    mov cl, v2
                    rol eax, cl
                    mov v1, eax
                }

                eflags->value = __readeflags();

                *(uint32_t*)dst = v1;
                break;
            }
            case sizeof(uint16_t) : {
                uint16_t v1 = *(uint16_t*)dst;
                uint8_t v2 = *src;

                __asm {
                    mov ax, v1
                    mov cl, v2
                    rol ax, cl
                    mov v1, ax
                }

                eflags->value = __readeflags();

                *(uint16_t*)dst = v1;
                break;
            }
            case sizeof(uint8_t) : {
                uint8_t v1 = *dst;
                uint8_t v2 = *src;

                __asm {
                    mov al, v1
                    mov cl, v2
                    rol al, cl
                    mov v1, al
                }

                eflags->value = __readeflags();

                *dst = v1;
                break;
            }
        }
    }
};