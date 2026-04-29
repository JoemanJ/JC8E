#pragma once
#include <vector>
#include "commons.hpp"
#include "Idisplay.hpp"

constexpr uint16_t SCREEN_WIDTH = 64;
constexpr uint16_t SCREEN_HEIGHT = 32;
constexpr uint16_t SCREEN_SIZE = SCREEN_WIDTH*SCREEN_HEIGHT; //2048 

class Display : public IDisplay{
    friend class DisplayTest;

    private:
        std::vector<pixel_t> buffer;

        uint16_t width;
        uint16_t height;    
    public:
        // Initializes a completely black display
        Display(uint8_t width = SCREEN_WIDTH, uint8_t height = SCREEN_HEIGHT);

        // Returns the display's width
        const byte_t getWidth() const {return width;}
        
        // Returns the display's width
        const byte_t getHeight() const {return height;}

        const pixel_t getPixel(byte_t x, byte_t y) const {return buffer[width*y + x];} 

        // If the pixel on position (x,y) is on: turn it off. Otherwise, turn it on.
        void togglePixel(byte_t x, byte_t y) override {
            pixel_t& p = buffer.at(y*width + x);
            p = ~p;
        }
        /*
        If the pixel on position (index%SCREEN_WIDTH, index//SCREEN_WIDTH) is on:
        turn it off. Otherwise, turn it on.
        */
       void togglePixel(uint16_t index){
            pixel_t& p = buffer.at(index);
            p = ~p;
       }

       // Turns every píxel of the display black
       void clear();

       // Returns a vector with all pixels on the screen
       const std::vector<pixel_t>& getPixels() const {return buffer;} 
};