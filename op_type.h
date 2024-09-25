#pragma once
#include <iostream>

class OperandType {
	enum Type : uint8_t {
		IS_DEREF = 0,
		IS_REGISTER = 0b1,
		IS_REL_OFFSET = 0b10,
		IS_SIGNED_INT = 0b11,
	};

	Type type;
public:
	OperandType(Type type) {
		this->type = type;
	}

	template <typename T>
	OperandType(T flags, int bit_offset) {
		this->type = (Type)((flags >> bit_offset) & 0b11);
	}

	bool is_deref() {
		return type == IS_DEREF;
	}

	bool is_register() {
		return type == IS_REGISTER;
	}

	bool is_rel_offset() {
		return type == IS_REL_OFFSET;
	}

	bool is_signed_int() {
		return type == IS_SIGNED_INT;
	}
};