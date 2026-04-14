#pragma once
#include <inttypes.h>
#include <commons.hpp>

typedef uint8_t pixel_t;

class IDisplay{
    public:
        virtual ~IDisplay() = default;
        virtual void togglePixel(byte_t x, byte_t y) = 0;
        virtual void clear() = 0;
        virtual const byte_t getWidth() const = 0;
        virtual const byte_t getHeight() const = 0;
        virtual const pixel_t getPixel(byte_t x, byte_t y) const = 0;
};