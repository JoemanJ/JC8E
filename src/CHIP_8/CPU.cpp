#include <CHIP_8/CPU.hpp>
#include <algorithm>

using namespace std;

/* Creates a new CPU with:
memory:  standard font from 0x050 to 0x09F, al other addresses are 0x00
regs: all 0
PC: 0x200
I: 0*/
CPU::CPU(){
    memory = array<byte_t, RAM_SIZE>();
    // Copy font to memory
    for (unsigned short i = 0; i<(sizeof(FONT)/sizeof(FONT[0])); i++){
        memory[i+0x50] = FONT[i+0x50];
    }
    regs = array<byte_t, 16>();
    PC = 0x200;
    I = 0x0;
}

inline byte_t CPU::memRead(addr_t address) const{
    return memory.at(address);
}

void CPU::memWrite(addr_t address, byte_t value){
    memory[address] = value;
}
