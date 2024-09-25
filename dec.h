#pragma once
#include "instruction.h"
#include "single_op.h"

class DecInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        SingleOpFlags flags = *(SingleOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        int operand_sz;
        auto operand = flags.get_operand_ptr(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip, operand_sz);

        dec((uint8_t*)operand, &vm_data->eflags, operand_sz);

        vm_data->registers.ip += instruction_size;
    }

private:
    void dec(uint8_t* dst, EFlags* eflags, int op_size) {
        switch (op_size) {
            case sizeof(uint64_t) : {
                uint64_t v1 = *(uint64_t*)dst;

                __asm {
                    mov rax, v1
                    dec rax
                    mov v1, rax
                }

                eflags->value = __readeflags();

                *(uint64_t*)dst = v1;
                break;
            }
            case sizeof(uint32_t) : {
                uint32_t v1 = *(uint32_t*)dst;

                __asm {
                    mov eax, v1
                    dec eax
                    mov v1, eax
                }

                eflags->value = __readeflags();

                *(uint32_t*)dst = v1;
                break;
            }
            case sizeof(uint16_t) : {
                uint16_t v1 = *(uint16_t*)dst;

                __asm {
                    mov ax, v1
                    dec ax
                    mov v1, ax
                }

                eflags->value = __readeflags();

                *(uint16_t*)dst = v1;
                break;
            }
            case sizeof(uint8_t) : {
                uint8_t v1 = *(uint8_t*)dst;

                __asm {
                    mov al, v1
                    dec al
                    mov v1, al
                }

                eflags->value = __readeflags();

                *dst = v1;
                break;
            }
        }
    }
};