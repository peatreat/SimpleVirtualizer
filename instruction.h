#pragma once
#include <iostream>
#include <intrin.h>
#include "data.h"

const int OPCODE_SIZE = 1;

enum InstructionType : uint8_t {
	PUSH = 0,
	POP,
	ADD,
	SUB,
	MUL,
	DIV,
	MOV,
	LEA,
	CALL,
	RET,
	JMP,
	TEST,
	CMP,
	XOR,
	JE,
	JNE,
	JAE,
	INT3,
	STOS,
	JLE,
	MOVSX,
	INC,
	JL,
	DEC,
	MOVZX,
	AND,
	SHR,
	NEG,
	IMUL,
	CDQE,
	ROL,
	ROR,
	JNLE,
	SETNZ,
	JNBE,
	CMOVB,
	CMOVZ,
	CMOVS,
	JBE,
	JNL,
	SHL,
	NOT,
	OR,
	JB,
	CMPXCHG,
	MOVS,
	XCHG,
	SETZ,
	CMOVNZ,
	CPUID,
	XGETBV,
};

enum MulFlags : uint8_t {
	MUL2 = 0b1,
	MUL4 = 0b10,
	MUL8 = 0b11,
};

class Instruction {
public:
	virtual void execute(VmData* vm_data) = 0;

	template <typename T>
	inline bool has_flag(T flags, T flag) {
		return (flags & flag) == flag;
	}

	static inline void mul_by_flag(uintptr_t* value, MulFlags mul_flags) {
		switch (mul_flags) {
		case MUL8:
			*value *= 8;
			break;
		case MUL4:
			*value *= 4;
			break;
		case MUL2:
			*value *= 2;
			break;
		}
	}
};