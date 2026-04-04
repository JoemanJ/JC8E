#pragma once
#include "commons.hpp"
#include "IRAM.hpp"
#include <array>

inline constexpr addr_t RAM_SIZE = 4*1024; //4kB

class RAM : public IRAM{
    private:
        std::array<byte_t, RAM_SIZE> memory;

    public:
        // Initializes memory with all zeros
        RAM();

        // Reads byte from address
        byte_t read(addr_t address) const override {return memory.at(address);}

        // Writes a single byte of data to address
        void write(addr_t address, byte_t data) override {memory.at(address) = data;}
        
        // Retrieves a reference to a byte in memory
        byte_t &operator[](addr_t idx){return memory.at(idx);}
        
        // Writes a buffer of given size to memory, starting from startAddress
        void bulkWrite(addr_t startAddress, std::size_t size, const byte_t* data);
};