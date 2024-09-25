#pragma once
#include <iostream>

union EFlags {
    uint64_t value;

    struct {
        uint32_t CF : 1;
        uint32_t reserved : 1;
        uint32_t PF : 1;
        uint32_t reserved2 : 1;
        uint32_t AF : 1;
        uint32_t reserved3 : 1;
        uint32_t ZF : 1;
        uint32_t SF : 1;
        uint32_t TF : 1;
        uint32_t IF : 1;
        uint32_t DF : 1;
        uint32_t OF : 1;
        uint32_t IOPL : 2;
        uint32_t NT : 1;
        uint32_t MD : 1;
        uint32_t RF : 1;
        uint32_t VM : 1;
        uint32_t AC : 1;
        uint32_t VIF : 1;
        uint32_t VIP : 1;
        uint32_t ID : 1;
        uint32_t reserved4 : 8;
        uint32_t unkn : 1;
        uint32_t AI : 1;
    };
};