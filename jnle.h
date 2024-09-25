#pragma once
#include "jcc.h"

class JnleInstruction : JccInstruction {
public:
    JnleInstruction(VmData* vm_data) : JccInstruction(vm_data) {}

    void execute(VmData* vm_data) {
        if (vm_data->eflags.ZF == 0 && vm_data->eflags.SF == vm_data->eflags.OF) {
            jump(vm_data);
            return;
        }

        skip(vm_data);
    }
};