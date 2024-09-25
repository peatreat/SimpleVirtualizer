#pragma once
#include "instruction.h"
#include "op_type.h"

class SingleOpFlags {
public:
	enum Flags : uint8_t {
        IS_DEREF= 0,
        IS_REGISTER = 0b1,
        IS_REL_OFFSET = 0b10,
        IS_SIGNED_INT = 0b11,
        
        HAS_DISPLACEMENT = 0b100,
        HAS_BASE_REG = 0b1000,
        HAS_INDEX_REG = 0b10000,
        HAS_SCALE_MUL2 = 0b100000,
        HAS_SCALE_MUL4 = 0b1000000,
        HAS_SCALE_MUL8 = 0b1100000,
	};

	bool has_flag(uint8_t flag) {
		return (flags & flag) == flag;
	}

    bool equals(uint8_t flags) {
        return this->flags == flags;
    }

    OperandType get_op_type() {
        return OperandType(flags, 0);
    }

	uintptr_t get_operand(Registers* registers, int& instruction_size, uintptr_t ip, int& operand_sz) {
        auto op_type = OperandType(flags, 0);

        if (op_type.is_register()) {
            RegisterID reg = *(RegisterID*)(ip + instruction_size);

            instruction_size += sizeof(reg);

            operand_sz = reg.get_size();
            return registers->get_register<uintptr_t>(reg.get_index());
        }
        else if (op_type.is_signed_int()) {
            int signed_int = *(int*)(ip + instruction_size);

            instruction_size += sizeof(int);
            operand_sz = sizeof(int);

            return signed_int;
        }
        else if (op_type.is_rel_offset()) {
            int signed_rel_offset = *(int*)(ip + instruction_size);

            instruction_size += sizeof(int);
            operand_sz = sizeof(uintptr_t);
            return *(uintptr_t*)(ip + instruction_size + signed_rel_offset);
        }
        else if (op_type.is_deref()) {
            uintptr_t pointer = 0;
            operand_sz = sizeof(uintptr_t);

            bool is_rel = false;

            if (has_flag(HAS_BASE_REG)) {
                RegisterID register_index = *(RegisterID*)(ip + instruction_size);

                instruction_size += sizeof(register_index);

                pointer = registers->get_register<uint64_t>(register_index.get_index());

                if (register_index.get_index() == RIP)
                    is_rel = true;
            }

            if (has_flag(HAS_INDEX_REG)) {
                RegisterID index_data = *(RegisterID*)(ip + instruction_size);

                instruction_size += sizeof(index_data);

                uintptr_t reg = registers->get_register<uint64_t>(index_data.get_index());

                Instruction::mul_by_flag(&reg, (MulFlags)(flags >> 5));

                pointer += reg;
            }

            if (has_flag(HAS_DISPLACEMENT)) {
                pointer += *(int*)(ip + instruction_size);
                instruction_size += sizeof(int);
            }

            if (is_rel)
                pointer += instruction_size;

            return *(uintptr_t*)pointer;
        }

        return 0;
	}

    uintptr_t* get_operand_ptr(Registers* registers, int& instruction_size, uintptr_t ip, int& operand_sz) {
        auto op_type = OperandType(flags, 0);

        if (op_type.is_register()) {
            RegisterID reg = *(RegisterID*)(ip + instruction_size);

            instruction_size += sizeof(reg);
            operand_sz = reg.get_size();

            return (uintptr_t*)registers->get_register_ptr(reg.get_index());
        }
        else if (op_type.is_signed_int()) {
            instruction_size += sizeof(int);
            operand_sz = sizeof(int);
            return nullptr;
        }
        else if (op_type.is_rel_offset()) {
            int signed_rel_offset = *(int*)(ip + instruction_size);

            instruction_size += sizeof(int);
            operand_sz = sizeof(uintptr_t);

            return (uintptr_t*)(ip + instruction_size + signed_rel_offset);
        }
        else if (op_type.is_deref()) {
            uintptr_t pointer = 0;
            operand_sz = sizeof(uintptr_t);

            bool is_rel = false;

            if (has_flag(HAS_BASE_REG)) {
                RegisterID register_index = *(RegisterID*)(ip + instruction_size);

                instruction_size += sizeof(register_index);

                pointer = registers->get_register<uint64_t>(register_index.get_index());

                if (register_index.get_index() == RIP)
                    is_rel = true;
            }

            if (has_flag(HAS_INDEX_REG)) {
                RegisterID index_data = *(RegisterID*)(ip + instruction_size);

                instruction_size += sizeof(index_data);

                uintptr_t reg = registers->get_register<uint64_t>(index_data.get_index());

                Instruction::mul_by_flag(&reg, (MulFlags)(flags >> 5));

                pointer += reg;
            }

            if (has_flag(HAS_DISPLACEMENT)) {
                pointer += *(int*)(ip + instruction_size);
                instruction_size += sizeof(int);
            }

            if (is_rel)
                pointer += instruction_size;

            return (uintptr_t*)pointer;
        }

        return 0;
    }

private:
	Flags flags;
};