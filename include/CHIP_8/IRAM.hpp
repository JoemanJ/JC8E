#pragma once
#include "commons.hpp"

class IRAM{
    public:
        virtual void write(addr_t addrress, byte_t value) = 0;
        virtual byte_t read(addr_t address) const = 0;
        virtual void bulkWrite(addr_t startAddress, std::size_t size, const byte_t *data) = 0;
};