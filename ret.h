#pragma once
#include "instruction.h"
#include "pop.h"

class RetInstruction : Instruction {
public:
    void execute(VmData* vm_data) {
        PopInstruction pop;

        auto ret_address = pop.pop_ptr(vm_data);

        vm_data->registers.ip = (uint8_t*)ret_address;
    }
};