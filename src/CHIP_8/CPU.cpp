#include "CHIP_8/CPU.hpp"

using namespace std;
CPU::CPU(IRAM& ram, IDisplay& display): memory(ram), display(display){
    // Copy font to memory
    memory.bulkWrite(0x50, sizeof(FONT), FONT);
    
    // Initialize registers
    regs = std::array<byte_t, 16>();
    PC = 0x200;
    I = 0x0;
}

byte_t CPU::memRead(addr_t address) const{
    return memory.read(address);
}

void CPU::memWrite(addr_t address, byte_t value){
    memory.write(address, value);
}

uint16_t CPU::fetch()
{
    uint16_t instruction = (memory.read(PC) << 8) | memory.read(PC+1);
    PC += 2;
    return instruction;
}
