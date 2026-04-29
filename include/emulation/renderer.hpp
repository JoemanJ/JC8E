#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include "commons.hpp"

/*
    Class responsible for rendering the pixel data from a CHIP-8 Display to
    a SFML sprite.
*/
class Renderer{
    public:
        Renderer(
            uint16_t screenWidth,
            uint16_t screenHeight,
            float scale);
        
        // Updates the internal rendering buffer according to the given array of pixels
        void update(const std::vector<byte_t>& source);

        // Draws the internal rendering buffer to the SFML Window
        void draw(sf::RenderWindow& window) const;

    private:
        uint16_t screenWidth;
        uint16_t screenHeight;

        sf::Texture screenTexture;
        sf::Sprite screenSprite;

        std::array<sf::Uint8, 4*64*32> buffer;
        float scale;
};