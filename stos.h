#pragma once
#include "instruction.h"
#include "rep_flags.h"

class StosInstruction : Instruction {
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
        auto vm_ecx = vm_data->registers.get_register<uint32_t>(RCX);
        auto vm_eax = vm_data->registers.get_register<uint32_t>(RAX);

        auto vm_eflags = vm_data->eflags;

        switch (size) {
        case IS_BYTE >> 2:
            __asm {
                mov rdi, vm_rdi
                mov ecx, vm_ecx
                mov eax, vm_eax

                push vm_eflags
                popf

                cmp is_rep, 1
                je rep_loc

                stosb

                jmp done

            rep_loc:
                rep stosb

            done:
                mov vm_rdi, rdi
                mov vm_ecx, ecx
                mov vm_eax, eax
            }
            break;
        case IS_WORD >> 2:
            __asm {
                mov rdi, vm_rdi
                mov ecx, vm_ecx
                mov eax, vm_eax

                push vm_eflags
                popf

                cmp is_rep, 1
                je rep_loc

                stosw

                jmp done

            rep_loc :
                rep stosw

              done :
                mov vm_rdi, rdi
                mov vm_ecx, ecx
                mov vm_eax, eax
            }
            break;
        case IS_DWORD >> 2:
            __asm {
                mov rdi, vm_rdi
                mov ecx, vm_ecx
                mov eax, vm_eax

                push vm_eflags
                popf

                cmp is_rep, 1
                je rep_loc

                stosd

                jmp done

            rep_loc :
                rep stosd

            done :
                mov vm_rdi, rdi
                mov vm_ecx, ecx
                mov vm_eax, eax
            }
            break;
        case IS_QWORD >> 2:
            __asm {
                mov rdi, vm_rdi
                mov ecx, vm_ecx
                mov eax, vm_eax

                push vm_eflags
                popf

                cmp is_rep, 1
                je rep_loc

                stosq

                jmp done

             rep_loc :
                rep stosq

            done :
                mov vm_rdi, rdi
                mov vm_ecx, ecx
                mov vm_eax, eax
            }
            break;
        }

        vm_data->registers.set_register(RDI, vm_rdi);
        vm_data->registers.set_register(RCX, vm_ecx);
        vm_data->registers.set_register(RAX, vm_eax);

        vm_data->registers.ip += instruction_size;
    }
};