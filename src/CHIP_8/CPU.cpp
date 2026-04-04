#include <CHIP_8/CPU.hpp>
#include <algorithm>

using namespace std;
CPU::CPU(RAM& ram): memory(ram){
    // Copy font to memory
    memory.bulkWrite(0x50, FONT);
    
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
