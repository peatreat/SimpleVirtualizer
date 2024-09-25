#pragma once
#include "instruction.h"
#include "single_op.h"

class JmpInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        auto fff = _AddressOfReturnAddress();

        SingleOpFlags flags = *(SingleOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        uintptr_t jmp_loc = 0;

        auto op_type = flags.get_op_type();

        if (op_type.is_signed_int() || op_type.is_rel_offset()) {
            int ip_offset = *(int*)(vm_data->registers.ip + instruction_size);
            instruction_size += sizeof(int);

            jmp_loc = (uintptr_t)vm_data->registers.ip + instruction_size + ip_offset;
        }
        else {
            int operand_sz;
            jmp_loc = flags.get_operand(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip, operand_sz);
        }

        if (vm_data->is_function_virtualized(jmp_loc)) {
            vm_data->registers.ip = (uint8_t*)jmp_loc;
            return;
        }

        // handle non-virtualized function calls

        uintptr_t vm_rcx = vm_data->registers.get_register<uintptr_t>(Register::RCX);
        uintptr_t vm_rdx = vm_data->registers.get_register<uintptr_t>(Register::RDX);
        uintptr_t vm_rbx = vm_data->registers.get_register<uintptr_t>(Register::RBX);
        uintptr_t vm_rdi = vm_data->registers.get_register<uintptr_t>(Register::RDI);
        uintptr_t vm_rsi = vm_data->registers.get_register<uintptr_t>(Register::RSI);
        uintptr_t vm_r8 = vm_data->registers.get_register<uintptr_t>(Register::R8);
        uintptr_t vm_r9 = vm_data->registers.get_register<uintptr_t>(Register::R9);
        uintptr_t vm_r10 = vm_data->registers.get_register<uintptr_t>(Register::R10);
        uintptr_t vm_r11 = vm_data->registers.get_register<uintptr_t>(Register::R11);
        uintptr_t vm_r12 = vm_data->registers.get_register<uintptr_t>(Register::R12);
        uintptr_t vm_r13 = vm_data->registers.get_register<uintptr_t>(Register::R13);
        uintptr_t vm_r14 = vm_data->registers.get_register<uintptr_t>(Register::R14);
        uintptr_t vm_r15 = vm_data->registers.get_register<uintptr_t>(Register::R15);

        uintptr_t new_rsp = vm_data->registers.get_register<uintptr_t>(Register::RSP);

        size_t stack_size = vm_data->stack_base - new_rsp;

        bool added_padding = false;

        auto new_stack_base = vm_data->stack_base;

        uint64_t result, old_ret;

        // future optimization could be have a separate suspended thread and use its stack for our vm stack and whenever we need to do jmps to non-virtualized functions we push ret address, set rip with setthreadcontext, and resume thread. the return address should be something that does sets rax in vm ctx, and a lock cmpxchg to tell main thread we finished. in main thread once we are done spinning re-suspend the thread for future calls

        __asm {
            mov rax, rsp
            sub rax, stack_size
            and rax, 0xF
            jz run
            push 0
            mov byte ptr[added_padding], 1

        run:
            mov r8, new_stack_base
        loop :
            cmp r8, new_rsp
            je done
            sub r8, 8
            push qword ptr[r8]
            jmp loop

         done :
            mov rcx, vm_rcx
            mov rdx, vm_rdx
            mov rbx, vm_rbx
            mov rdi, vm_rdi
            mov rsi, vm_rsi
            mov r8, vm_r8
            mov r9, vm_r9
            mov r10, vm_r10
            mov r11, vm_r11
            mov r12, vm_r12
            mov r13, vm_r13
            mov r14, vm_r14
            mov r15, vm_r15

            mov rax, [rsp]
            mov qword ptr[old_ret], rax
            lea rax, [rip]
            add rax, 11
            mov [rsp], rax
            jmp qword ptr [jmp_loc]

            mov r11b, added_padding
            test r11b, r11b
            jz end
            add rsp, 8

        end:
            add rsp, stack_size
            mov result, rax
        }

        vm_data->registers.set_register(Register::RAX, result);

        vm_data->registers.ip = (uint8_t*)old_ret;
    }
};