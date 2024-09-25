#pragma once
#include "instruction.h"
#include "rep_flags.h"

class MovsInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        REPFlags flags = *(REPFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        REPFlags rep_type = (REPFlags)((uint8_t)flags & 0b11);

        if (rep_type != REP && rep_type != NO_REP)
            throw std::runtime_error("unsupported rep type for stos");

        bool is_rep = rep_type == REP;

        REPFlags size = (REPFlags)(((uint8_t)flags >> 2) & 0b1111);

        auto vm_rdi = vm_data->registers.get_register<uintptr_t>(RDI);
        auto vm_rcx = vm_data->registers.get_register<uintptr_t>(RCX);
        auto vm_rax = vm_data->registers.get_register<uintptr_t>(RAX);
        auto vm_rsi = vm_data->registers.get_register<uintptr_t>(RSI);

        auto vm_eflags = vm_data->eflags;

        switch (size) {
        case IS_BYTE >> 2:
            __asm {
                mov rdi, vm_rdi
                mov rcx, vm_rcx
                mov rax, vm_rax
                mov rsi, vm_rsi

                push vm_eflags
                popf

                cmp is_rep, 1
                je rep_loc

                movsb

                jmp done

                rep_loc :
                rep movsb

             done :
                mov vm_rdi, rdi
                mov vm_rcx, rcx
                mov vm_rax, rax
                mov vm_rsi, rsi
            }
            break;
        case IS_WORD >> 2:
            __asm {
                mov rdi, vm_rdi
                mov rcx, vm_rcx
                mov rax, vm_rax
                mov rsi, vm_rsi

                push vm_eflags
                popf

                cmp is_rep, 1
                je rep_loc

                movsw

                jmp done

                rep_loc :
                rep movsw

             done :
                mov vm_rdi, rdi
                mov vm_rcx, rcx
                mov vm_rax, rax
                mov vm_rsi, rsi
            }
            break;
        case IS_DWORD >> 2:
            __asm {
                mov rdi, vm_rdi
                mov rcx, vm_rcx
                mov rax, vm_rax
                mov rsi, vm_rsi

                push vm_eflags
                popf

                cmp is_rep, 1
                je rep_loc

                movsd

                jmp done

                rep_loc :
                rep movsd

             done :
                mov vm_rdi, rdi
                mov vm_rcx, rcx
                mov vm_rax, rax
                mov vm_rsi, rsi
            }
            break;
        case IS_QWORD >> 2:
            __asm {
                mov rdi, vm_rdi
                mov rcx, vm_rcx
                mov rax, vm_rax
                mov rsi, vm_rsi

                push vm_eflags
                popf

                cmp is_rep, 1
                je rep_loc

                movsq

                jmp done

                rep_loc :
                rep movsq

             done :
                mov vm_rdi, rdi
                mov vm_rcx, rcx
                mov vm_rax, rax
                mov vm_rsi, rsi
            }
            break;
        }

        vm_data->registers.set_register(RDI, vm_rdi);
        vm_data->registers.set_register(RCX, vm_rcx);
        vm_data->registers.set_register(RAX, vm_rax);
        vm_data->registers.set_register(RSI, vm_rsi);

        vm_data->registers.ip += instruction_size;
    }
};