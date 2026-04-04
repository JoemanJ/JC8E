#pragma once
#include <array>
#include "commons.hpp"

constexpr uint16_t SCREEN_WIDTH = 64;
constexpr uint16_t SCREEN_HEIGHT = 32;
constexpr uint16_t SCREEN_SIZE = SCREEN_WIDTH*SCREEN_HEIGHT; //2048 

class Display{
    friend class DisplayTest;

    private:
        std::array<byte_t, SCREEN_SIZE> buffer;
    
    public:
        // If the pixel on position (x,y) is on: turn it off. Otherwise, turn it on.
        void togglePixel(byte_t x, byte_t y){
            buffer[y*SCREEN_WIDTH + x] = ~buffer[y*SCREEN_WIDTH+x];
        }
        /*
        If the pixel on position (index%SCREEN_WIDTH, index//SCREEN_WIDTH) is on:
        turn it off. Otherwise, turn it on.
        */
       void togglePixel(uint16_t index){
            buffer[index] = ~buffer[index];
       }
};