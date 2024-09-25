#pragma once
#include "jcc.h"

class JbeInstruction : JccInstruction {
public:
    JbeInstruction(VmData* vm_data) : JccInstruction(vm_data) {}

    void execute(VmData* vm_data) {
        if (vm_data->eflags.CF || vm_data->eflags.ZF) {
            jump(vm_data);
            return;
        }

        skip(vm_data);
    }
};