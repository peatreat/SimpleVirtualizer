#pragma once
#include <iostream>
#include <vector>

#include "register.h"
#include "eflags.h"

class VmData {
public:
    uintptr_t stack_base;

    Registers registers;
    EFlags eflags;

    uintptr_t assembly_start, assembly_end;

public:
    bool is_function_virtualized(uintptr_t function) {
        return function >= assembly_start && function < assembly_end;
    }
};