#pragma once
#include "instruction.h"

class JccInstruction : Instruction {
protected:
    enum Flags : uint8_t {
        SIGNED_WORD = 0,
        SIGNED_REL_OFFSET = 0b1,
    };

    Flags flags;
public:
    JccInstruction(VmData* vm_data) {
        this->flags = *(Flags*)(vm_data->registers.ip + OPCODE_SIZE);
    }

    void jump(VmData* vm_data) {
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        uintptr_t jmp_loc = 0;

        if (flags == SIGNED_WORD) {
            int16_t ip_offset = *(int8_t*)(vm_data->registers.ip + instruction_size);

            instruction_size += sizeof(ip_offset);

            jmp_loc = (uintptr_t)vm_data->registers.ip + instruction_size + ip_offset;
        }
        else if (flags == SIGNED_REL_OFFSET) {
            int ip_offset = *(int*)(vm_data->registers.ip + instruction_size);

            instruction_size += sizeof(int);

            jmp_loc = (uintptr_t)vm_data->registers.ip + instruction_size + ip_offset;
        }
        else {
            throw std::runtime_error("unhandled jcc instruction");
        }

        vm_data->registers.ip = (uint8_t*)jmp_loc;
    }

    void skip(VmData* vm_data) {
        int instruction_size = OPCODE_SIZE + sizeof(flags);

        if (flags == SIGNED_WORD) {
            instruction_size += sizeof(int16_t);
        }
        else if (flags == SIGNED_REL_OFFSET) {
            instruction_size += sizeof(int);
        }

        vm_data->registers.ip += instruction_size;
    }

    virtual void execute(VmData* vm_data) = 0;
};