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
    friend class RendererTest;

    public:
        Renderer(
            uint16_t screenWidth,
            uint16_t screenHeight,
            float scale
        );
        
        // Updates the internal rendering buffer according to the given array of pixels
        void update(const std::vector<byte_t>& source);

        // Draws the internal rendering buffer to the SFML Window
        void draw(sf::RenderWindow& window) const;

    protected:
        uint16_t screenWidth;
        uint16_t screenHeight;

        sf::Texture screenTexture;
        sf::Sprite screenSprite;

        std::vector<sf::Uint8> buffer; // RGBA pixel buffer
        float scale;

        /*
        Converts the pixel format from the CHIP 8 display format (single byte_t,
        0 for an OFF pixel, 255 for an ON pixel) to the format expected by SFML
        (4 sf::uint_8, RGBA) and copies it to the internal buffer
        */
        void convertPixelsFromChip8(const std::vector<byte_t>& source);
};