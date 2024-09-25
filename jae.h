#pragma once
#include "jcc.h"

class JaeInstruction : JccInstruction {
public:
    JaeInstruction(VmData* vm_data) : JccInstruction(vm_data) {}

    void execute(VmData* vm_data) {
        if (vm_data->eflags.CF == 0) {
            jump(vm_data);
            return;
        }

        skip(vm_data);
    }
};