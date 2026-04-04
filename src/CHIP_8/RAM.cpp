#include "CHIP_8/RAM.hpp"

RAM::RAM(){
    memory.fill(0x00);
}

void RAM::bulkWrite(addr_t startAddress, std::size_t size, const byte_t *data){
    for(addr_t i=0; i<size; i++){
        memory.at(startAddress+i) = data[i];
    }
}
