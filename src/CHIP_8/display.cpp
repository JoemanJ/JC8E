#include "CHIP_8/display.hpp"

#include <iostream>
Display::Display(uint8_t width, uint8_t height):
width(width),
height(height)
{
    uint16_t totalPixels = width*height;
    buffer.reserve(totalPixels);
    for (uint16_t i = 0; i < totalPixels; i++){
        buffer.push_back(0x00); // All pixels start off
    }
}

void Display::clear(){
    for (pixel_t& p: buffer){
        p = 0x00;
    }
}