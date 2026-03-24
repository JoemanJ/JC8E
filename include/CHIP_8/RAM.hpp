#pragma once
#include "commons.hpp"
#include <array>

inline constexpr addr_t RAM_SIZE = 4*1024;

class RAM{
    private:
        std::array<byte_t, RAM_SIZE> memory;

    public:
        // Reads byte from address
        byte_t read(addr_t address){
            return memory.at(address);
        }

        // Writes a single byte of data to address
        void write(addr_t address, byte_t data){
            memory.at(address) = data;
        }
        
        // Retrieves a reference to a byte in memory
        byte_t &operator[](addr_t idx){
            return memory.at(idx);
        }
        
        // Writes a buffer to memory, starting from startAddress
        template <typename T>
        void bulkWrite(addr_t startAddress, addr_t size, const T &data){
            const byte_t* source = std::data(data);

            for(addr_t i=0; i<size; i++)
            {
                memory[startAddress+i] = source[i];
            }
        }
        // Writes 'size' bytes to memory, starting from startAddress
        template <typename T>
        void bulkWrite(addr_t startAddress, const T &data){
            const byte_t* source = std::data(data);

            std::copy(std::begin(data), std::end(data), memory.begin()+startAddress);
        }
};
