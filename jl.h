#pragma once
#include "jcc.h"

class JlInstruction : JccInstruction {
public:
    JlInstruction(VmData* vm_data) : JccInstruction(vm_data) {}

    void execute(VmData* vm_data) {
        if (vm_data->eflags.SF != vm_data->eflags.OF) {
            jump(vm_data);
            return;
        }

        skip(vm_data);
    }
};