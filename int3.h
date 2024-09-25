#pragma once
#include "instruction.h"

class Int3Instruction : Instruction {
public:
    void execute(VmData* vm_data) {
        __debugbreak();
    }
};