#pragma once
#include <inttypes.h>

class IDisplay{
    public:
        virtual ~IDisplay() = default;
        virtual void togglePixel(byte_t x, byte_t y) = 0;
        virtual void clear() = 0;
};