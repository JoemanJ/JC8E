#include <CHIP_8/CPU.hpp>
#include <algorithm>

using namespace std;

byte_t CPU::memRead(addr_t address) const{
    return memory.read(address);
}

void CPU::memWrite(addr_t address, byte_t value){
    memory.write(address, value);
}
