#pragma once
#include "instruction.h"
#include "two_op.h"

class CmpxchgInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        TwoOpFlags flags = *(TwoOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        auto result = flags.get_operands(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip);

        cmpxchg(result.op1, result.op2, (uint64_t*)vm_data->registers.get_register_ptr(RAX), &vm_data->eflags, result.operation_size);

        vm_data->registers.ip += instruction_size;
    }

private:
    void cmpxchg(uint8_t* dst, uint8_t* src, uint64_t* vm_rax, EFlags* eflags, int op_size) {
        uint64_t vm_rax_value = *vm_rax;

        switch (op_size) {
            case sizeof(uint64_t) : {
                uint64_t v2 = *(uint64_t*)src;

                __asm {
                    mov rax, vm_rax_value
                    mov rbx, v2
                    mov r11, dst
                    cmpxchg qword ptr[r11], rbx
                    mov vm_rax_value, rax
                }

                eflags->value = __readeflags();
                break;
            }
            case sizeof(uint32_t) : {
                uint32_t v2 = *(uint32_t*)src;

                __asm {
                    mov rax, vm_rax_value
                    mov ebx, v2
                    mov r11, dst
                    cmpxchg dword ptr[r11], ebx
                    mov vm_rax_value, rax
                }

                eflags->value = __readeflags();
                break;
            }
            case sizeof(uint16_t) : {
                uint16_t v2 = *(uint16_t*)src;

                __asm {
                    mov rax, vm_rax_value
                    mov bx, v2
                    mov r11, dst
                    cmpxchg word ptr[r11], bx
                    mov vm_rax_value, rax
                }

                eflags->value = __readeflags();
                break;
            }
            case sizeof(uint8_t) : {
                uint8_t v2 = *(uint16_t*)src;

                __asm {
                    mov rax, vm_rax_value
                    mov bl, v2
                    mov r11, dst
                    cmpxchg byte ptr[r11], bl
                    mov vm_rax_value, rax
                }

                eflags->value = __readeflags();
                break;
            }
        }

        *vm_rax = vm_rax_value;
    }
};