#include "loader.h"

int main() {
    //uint16_t add_flags = TwoOpFlags::HAS_BASE_REG | TwoOpFlags::SRC_IS_SIGNED_INT;

    //uint16_t mov_flags = TwoOpFlags::DST_IS_REGISTER | TwoOpFlags::SRC_IS_REGISTER;

    //int signed_num = 0x20;

    //std::vector<uint8_t> assembly{
    //    //InstructionType::PUSH, SingleOpFlags::IS_REGISTER, Register::RCX,
    //    //InstructionType::POP, SingleOpFlags::HAS_REG, Register::RDX,
    //    //InstructionType::ADD, LOBYTE(add_flags), HIBYTE(add_flags), Register::RDX, LOBYTE(signed_num), LOBYTE(signed_num >> 8), LOBYTE(signed_num >> 16), HIBYTE(signed_num >> 16),
    //    0x0c, 0x05, 0x00, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x01, 0x01, 0x0d, 0x00, 0x01, 0x09, 0x0c, 0x05, 0x00, 0x02, 0x02, 0x0c, 0x05, 0x00, 0x03, 0x03, 0x0c, 0x05, 0x00, 0x09, 0x09, 0x0c, 0x05, 0x00, 0x0a, 0x0a, 0x08, 0x01, 0x0c
    //    //0x03, 0x0d, 0x00, 0x05, 0x28, 0x00, 0x00, 0x00, 0x0c, 0x05, 0x00, 0x02, 0x02, 0x0c, 0x05, 0x00, 0x03, 0x03, 0x0c, 0x05, 0x00, 0x09, 0x09, 0x0c, 0x05, 0x00, 0x0a, 0x0a, 0x08, 0x01, 0x01, 0x02, 0x0d, 0x00, 0x05, 0x28, 0x00, 0x00, 0x00, 0x09
    //    //0x02, 0x0d, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00,
    //    ///InstructionType::MOV, LOBYTE(mov_flags), HIBYTE(mov_flags), Register::RCX, Register::R13,
    //    //InstructionType::MOV, LOBYTE(mov_flags), HIBYTE(mov_flags), Register::RDX, Register::R13,
    //    ///InstructionType::MOV, LOBYTE(mov_flags), HIBYTE(mov_flags), Register::R8, Register::R13,
    //    ///InstructionType::MOV, LOBYTE(mov_flags), HIBYTE(mov_flags), Register::R9, Register::R13,

    //    ///InstructionType::CALL, SingleOpFlags::IS_REGISTER, Register::R12,
    //};

    //VirtualMachine vm(assembly, entrypoint);

    //uintptr_t result = 0;

    //vm.registers.set_register(Register::RCX, (uint64_t)0x1337);
    ///vm.data()->registers.set_register(Register::RCX, (uint64_t)0x7fffffff);
    //vm.registers.set_register(Register::RDX, &result);
    ///vm.data()->registers.set_register(Register::RDX, "hello");
    ///vm.data()->registers.set_register(Register::R12, (uintptr_t)MessageBoxA);
    ///vm.data()->registers.set_register(Register::R13, (uintptr_t)0);

    //vm.data()->registers.set_register(Register::RAX, (uintptr_t)MessageBoxA);
    //vm.data()->registers.set_register(Register::R11, (uintptr_t)MessageBoxA);

    //vm.execute();

    //std::cout << "rax: " << vm.data()->registers.get_register<uintptr_t>(Register::RAX) << std::endl;

    //std::cout << "result: " << std::hex << result << std::endl;
    //std::cout << "cf: " << vm.data()->eflags.CF << std::endl;
    //std::cout << "of: " << vm.data()->eflags.OF << std::endl;
    //std::cout << "sf: " << vm.data()->eflags.SF << std::endl;
    //std::cout << "af: " << vm.data()->eflags.AF << std::endl;

    Loader loader;

    loader.load("C:\\Users\\Peatreat\\source\\repos\\SimpleAssembler\\x64\\Debug\\virtualized.dll");

    return 0;
}

/*
todo
    * add support for lock prefix instructions
    * add support for rep ret
    * add support for segment registers
    * fix relocations to go to virtualized section
*/