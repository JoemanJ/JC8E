#pragma once
#include "commons.hpp"
#include <string>
#include <filesystem>

class IRAM{
    public:
        // Reads byte from address
        virtual void write(addr_t addrress, byte_t value) = 0;

        // Writes a single byte of data to address
        virtual byte_t read(addr_t address) const = 0;

        // Writes a buffer of given size to memory, starting from startAddress
        virtual void bulkWrite(addr_t startAddress, std::size_t size, const byte_t *data) = 0;

        // Loads a file from the given path to ram
        virtual void load(const std::filesystem::path& path) = 0;
};