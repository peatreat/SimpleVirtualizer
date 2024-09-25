#pragma once
#include <iostream>

enum REPFlags : uint8_t {
	NO_REP = 0,
	REP = 0b1,
	REPE = 0b10,
	REPNE = 0b11,

	IS_BYTE = 0b100,
	IS_WORD = 0b1000,
	IS_DWORD = 0b10000,
	IS_QWORD = 0b100000,
};