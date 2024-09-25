#pragma once
#include "instruction.h"
#include "op_type.h"

struct TwoOpResult {
	uint8_t* op1, * op2;
	int operation_size;
	int dst_sz;
	bool src_is_imm;
};

class TwoOpFlags {
public:
	enum Flags : uint16_t {
		DST_IS_REGISTER = 0b1,
		DST_IS_REL_OFFSET = 0b10,
		SRC_IS_REGISTER = 0b100,
		SRC_IS_IS_REL_OFFSET = 0b1000,
		SRC_IS_SIGNED_INT = 0b1100,
		IS_IMM64 = 0b10000,

		HAS_SEGMENT_GS = 0b100000,
		HAS_DISPLACEMENT = 0b1000000,
		HAS_BASE_REG = 0b10000000,
		HAS_INDEX_REG = 0b100000000,
		HAS_SCALE_MUL2 = 0b1000000000,
		HAS_SCALE_MUL4 = 0b10000000000,
		HAS_SCALE_MUL8 = 0b11000000000,
	};

	bool has_flag(uint16_t flag) {
		return (flags & flag) == flag;
	}

	TwoOpResult get_operands(Registers* registers, int& instruction_size, uintptr_t ip) {
		uint8_t* dst = 0;
		uint8_t* src = 0;

		bool mem_is_rel = false;
		int dst_ip_offset = 0;

		int operation_size = sizeof(uintptr_t);

		int dst_sz;

		auto dst_op_type = OperandType(flags, 0);
		auto src_op_type = OperandType(flags, 2);

		if (dst_op_type.is_register()) {
			RegisterID reg = *(RegisterID*)(ip + instruction_size);

			instruction_size += sizeof(reg);

			operation_size = reg.get_size();
			dst_sz = operation_size;
			dst = registers->get_register_ptr(reg.get_index());
		}
		else if (dst_op_type.is_rel_offset()) {
			dst_ip_offset = *(int*)(ip + instruction_size);
			dst_sz = sizeof(uintptr_t);
			instruction_size += sizeof(int);
		}
		else if (dst_op_type.is_deref()) {
			dst_sz = sizeof(uintptr_t);
			dst = get_operand_ptr(registers, instruction_size, ip, mem_is_rel);
		}

		if (src_op_type.is_register()) {
			RegisterID reg = *(RegisterID*)(ip + instruction_size);

			instruction_size += sizeof(reg);

			operation_size = reg.get_size();
			src = registers->get_register_ptr(reg.get_index());
		}
		else if (src_op_type.is_rel_offset()) {
			int src_ip_offset = *(int*)(ip + instruction_size);
			instruction_size += sizeof(int);

			src = (uint8_t*)ip + instruction_size + src_ip_offset;
		}
		else if (src_op_type.is_signed_int()) {
			src = (uint8_t*)(ip + instruction_size);

			operation_size = has_flag(IS_IMM64) ? sizeof(int64_t) : sizeof(int);
			instruction_size += operation_size;
		}
		else if (src_op_type.is_deref()) {
			src = get_operand_ptr(registers, instruction_size, ip, mem_is_rel);
		}

		if (dst_op_type.is_rel_offset()) {
			dst = (uint8_t*)ip + instruction_size + dst_ip_offset;
		}

		if (mem_is_rel) {
			if (dst_op_type.is_deref())
				dst += instruction_size;
			else
				src += instruction_size;
		}

		return TwoOpResult{ dst, src, operation_size, dst_sz, src_op_type.is_signed_int() || src_op_type.is_rel_offset() };
	}

private:
	uint8_t* get_operand_ptr(Registers* registers, int& instruction_size, uintptr_t ip, bool& mem_is_rel) {
		uint8_t* ptr = nullptr;

		if (has_flag(HAS_SEGMENT_GS))
			ptr = (uint8_t*)__readgsqword(0x30);

		if (has_flag(HAS_BASE_REG)) {
			RegisterID reg = *(RegisterID*)(ip + instruction_size);

			instruction_size += sizeof(reg);

			ptr = registers->get_register<uint8_t*>(reg.get_index());

			if (reg.get_index() == RIP)
				mem_is_rel = true;
		}

		if (has_flag(HAS_INDEX_REG)) {
			RegisterID index_data = *(RegisterID*)(ip + instruction_size);

			instruction_size += sizeof(index_data);

			uintptr_t reg = registers->get_register<uintptr_t>(index_data.get_index());

			Instruction::mul_by_flag((uintptr_t*)&reg, (MulFlags)(flags >> 9));

			ptr += reg;
		}

		if (has_flag(HAS_DISPLACEMENT)) {
			ptr += *(int*)(ip + instruction_size);
			instruction_size += sizeof(int);
		}

		return ptr;
	}

private:
	Flags flags;
};