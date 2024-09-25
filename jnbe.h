#pragma once
#include "jcc.h"

class JnbeInstruction : JccInstruction {
public:
    JnbeInstruction(VmData* vm_data) : JccInstruction(vm_data) {}

    void execute(VmData* vm_data) {
        if (vm_data->eflags.ZF == 0 && vm_data->eflags.CF == 0) {
            jump(vm_data);
            return;
        }

        skip(vm_data);
    }
};