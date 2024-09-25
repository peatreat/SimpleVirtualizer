#pragma once
#include "jcc.h"

class JbInstruction : JccInstruction {
public:
    JbInstruction(VmData* vm_data) : JccInstruction(vm_data) {}

    void execute(VmData* vm_data) {
        if (vm_data->eflags.CF) {
            jump(vm_data);
            return;
        }

        skip(vm_data);
    }
};