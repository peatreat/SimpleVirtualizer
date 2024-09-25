#pragma once
#include "jcc.h"

class JeInstruction : JccInstruction {
public:
    JeInstruction(VmData* vm_data) : JccInstruction(vm_data) {}

    void execute(VmData* vm_data) {
        if (vm_data->eflags.ZF) {
            jump(vm_data);
            return;
        }

        skip(vm_data);
    }
};