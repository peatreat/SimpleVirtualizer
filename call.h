#pragma once
#include "instruction.h"
#include "single_op.h"
#include "push.h"

void dummy(uint64_t rcx, uint64_t rdx, uint64_t r8, uint64_t r9);

class CallInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        SingleOpFlags flags = *(SingleOpFlags*)(vm_data->registers.ip + OPCODE_SIZE);
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        //if ((uintptr_t)(vm_data->registers.ip - vm_data->assembly_start) == 0x1385)
            printf("%p\n", vm_data->registers.ip - vm_data->assembly_start);

        if ((uintptr_t)(vm_data->registers.ip - vm_data->assembly_start) == 0x3B45)
            printf("a");

        uintptr_t function = 0;

        auto op_type = flags.get_op_type();

        if (op_type.is_signed_int() || op_type.is_rel_offset()) {
            int ip_offset = *(int*)(vm_data->registers.ip + instruction_size);
            instruction_size += sizeof(int);

            function = (uintptr_t)vm_data->registers.ip + instruction_size + ip_offset;
        }
        else {
            int operand_sz;
            function = flags.get_operand(&vm_data->registers, instruction_size, (uintptr_t)vm_data->registers.ip, operand_sz);
        }

        // if function to call is virtualized then handle it by pushing ret address and changing ip
        if (vm_data->is_function_virtualized(function)) {
            uintptr_t ret_address = (uintptr_t)vm_data->registers.ip + instruction_size;

            PushInstruction push;
            push.push_ptr(vm_data, ret_address);

            vm_data->registers.ip = (uint8_t*)function;
            return;
        }
        
        // handle non-virtualized function calls

        uintptr_t a1 = vm_data->registers.get_register<uintptr_t>(Register::RCX);
        uintptr_t a2 = vm_data->registers.get_register<uintptr_t>(Register::RDX);
        uintptr_t a3 = vm_data->registers.get_register<uintptr_t>(Register::R8);
        uintptr_t a4 = vm_data->registers.get_register<uintptr_t>(Register::R9);

        uintptr_t new_rsp = vm_data->registers.get_register<uintptr_t>(Register::RSP);

        size_t stack_size = vm_data->stack_base - new_rsp;

        uintptr_t result;

        bool added_padding = false;

        auto new_stack_base = vm_data->stack_base;

        // future optimization could be have a separate suspended thread and use its stack for our vm stack and whenever we need to do calls to non-virtualized functions we push ret address, set rip with setthreadcontext, and resume thread. the return address should be something that does sets rax in vm ctx, and a lock cmpxchg to tell main thread we finished. in main thread once we are done spinning re-suspend the thread for future calls

        __asm {
            //push rcx
            //push rdx
            //push r8
            //push r9

            mov rax, rsp
            sub rax, stack_size
            and rax, 0xF
            jz run
            push 0
            mov byte ptr [added_padding], 1

        run:

            mov r8, new_stack_base
        loop:
            cmp r8, new_rsp
            je done
            sub r8, 8
            push qword ptr [r8]
            jmp loop

        done:
            //mov rax, stack_size
            //mov rax, rsp
            //sub rax, stack_size
            //mov rax, alignment_padding
            //sub rax, alignment_padding

            //mov rcx, rax
            //mov rdx, new_rsp
            //mov r8, stack_size

            //call memcpy

            //mov rsp, rax

            mov rcx, a1
            mov rdx, a2
            mov r8, a3
            mov r9, a4

            //mov rax, function
            //mov r11, alignment_padding
            //mov r12, new_rsp
            //mov r13, stack_size

            //mov rsp, r12

            //sub rsp, r11

            call function

            mov r11b, added_padding
            test r11b, r11b
            jz end
            add rsp, 8

            //mov rsp, rbp

            //add rsp, alignment_padding

            //add rsp, stack_size
        end:
            add rsp, stack_size
            mov result, rax

            //pop r9
            //pop r8
            //pop rdx
            //pop rcx
        }

        vm_data->registers.set_register(Register::RAX, result);

        vm_data->registers.ip += instruction_size;
    }
};