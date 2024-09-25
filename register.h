#pragma once
#include <iostream>
#include <immintrin.h>

struct Vec128 {
    float x1, x2, x3, x4;
};

struct Vec256 {
    Vec128 xmm, upper;
};

enum RegisterSize {
    LongLong = 0,
    Short = 0b1,
    Dword = 0b10,
    Byte = 0b11,
};

enum YmmRegisterSize {
    x256 = 0,
    x128 = 0b1,
};

enum Register : uint8_t {
    INVALID = 0,
    RAX,
    RCX,
    RDX,
    RBX,
    RSP,
    RBP,
    RSI,
    RDI,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    YMM0,
    YMM1,
    YMM2,
    YMM3,
    YMM4,
    YMM5,
    YMM6,
    YMM7,
    YMM8,
    YMM9,
    YMM10,
    YMM11,
    YMM12,
    YMM13,
    YMM14,
    YMM15,

    RIP,
};

class RegisterID {
    union {
        struct {
            uint8_t index : 6;
            uint8_t size : 2;
        };
    };

public:
    RegisterID(uint8_t index, RegisterSize size) {
        this->index = index;
        this->size = size;
    }

    RegisterID(uint8_t index) {
        this->index = index;
        this->size = LongLong;
    }

    int get_index() {
        return this->index;
    }

    bool is_ymm_register() {
        return this->index >= YMM0 && this->index <= YMM15;
    }

    int get_size() {
        if (this->is_ymm_register()) {
            return ((this->size == YmmRegisterSize::x128) ? sizeof(Vec128) : sizeof(Vec256));
        }

        switch (this->size) {
            case RegisterSize::LongLong:
                return sizeof(uintptr_t);
            case RegisterSize::Dword:
                return sizeof(uint32_t);
            case RegisterSize::Short:
                return sizeof(uint16_t);
            case RegisterSize::Byte:
                return sizeof(uint8_t);
        }
    }
};

class Registers {
    uintptr_t registers[16];
    Vec256 ymm_registers[16];
public:
    uint8_t* ip;
    template <typename T>
    void set_register(uint8_t index, T value) {
        if (index == RIP) {
            ip = (uint8_t*)value;
            return;
        }

        T* reg = (index >= YMM0 && index <= YMM15) ? ((T*)&ymm_registers[index - YMM0]) : ((T*)&registers[index - 1]);

        switch (sizeof(T)) {
        case sizeof(Vec256) :
            *(Vec256*)reg = *reinterpret_cast<Vec256*>(&value);
            break;
        case sizeof(Vec128) :
            *(Vec128*)reg = *reinterpret_cast<Vec128*>(&value);
            break;
        case sizeof(uintptr_t) :
            *(uintptr_t*)reg = *reinterpret_cast<uintptr_t*>(&value);
            break;
        case sizeof(uint32_t) :
            *(uint32_t*)reg = *reinterpret_cast<uint32_t*>(&value);
            break;
        case sizeof(uint16_t):
            *(uint16_t*)reg = *reinterpret_cast<uint16_t*>(&value);
            break;
        case sizeof(uint8_t):
            *(uint8_t*)reg = *reinterpret_cast<uint8_t*>(&value);
            break;
        }
    }

    template <typename T>
    T get_register(uint8_t index) {
        if (index == RIP)
            return *reinterpret_cast<T*>(&ip);

        T* reg = (index >= YMM0 && index <= YMM15) ? ((T*)&ymm_registers[index - YMM0]) : ((T*)&registers[index - 1]);

        return *reg;
    }

    uint8_t* get_register_ptr(uint8_t index) {
        if (index == RIP)
            return reinterpret_cast<uint8_t*>(&ip);

        return (index >= YMM0 && index <= YMM15) ? ((uint8_t*)&ymm_registers[index - YMM0]) : ((uint8_t*)&registers[index - 1]);
    }
};