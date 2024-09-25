#pragma once
#include "jcc.h"

class JneInstruction : JccInstruction {
public:
    JneInstruction(VmData* vm_data) : JccInstruction(vm_data) {}

    void execute(VmData* vm_data) {
        if (vm_data->eflags.ZF == 0) {
            jump(vm_data);
            return;
        }

        skip(vm_data);
    }
};