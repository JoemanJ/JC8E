#include "CHIP_8/RAM.hpp"
#include <string>
#include <fstream>
#include <vector>

using namespace std;

RAM::RAM(){
    memory.fill(0x00);
}

void RAM::bulkWrite(addr_t startAddress, size_t size, const byte_t *data){
    for(addr_t i=0; i<size; i++){
        memory.at(startAddress+i) = data[i];
    }
}

void RAM::load(const filesystem::path& path){
    ifstream romFile(path, ios::binary|ios::ate);
    if(romFile.is_open()){
        streamsize size = romFile.tellg();

        romFile.seekg(0, ios::beg);

        vector<char> buffer(size);
        romFile.close();

        if(romFile.read(buffer.data(), size)){
            bulkWrite(0x200, size, (byte_t*)buffer.data());
        }
    }
}