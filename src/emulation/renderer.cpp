#include "emulation/renderer.hpp"

using namespace std;

Renderer::Renderer(uint16_t screenWidth, uint16_t screenHeight, float scale):
scale(scale), screenWidth(screenWidth), screenHeight(screenHeight)
{
    screenTexture.create(screenWidth, screenHeight);
    screenSprite.setTexture(screenTexture);
    screenSprite.setScale(sf::Vector2(scale, scale));

    buffer.fill(255);
}

void Renderer::update(const vector<byte_t>& source){
    for(uint16_t i=0; i<screenWidth*screenHeight; i++){
        byte_t pixel = source.at(i);
        buffer.at(i*4 + 0) = pixel;
        buffer.at(i*4 + 1) = pixel;
        buffer.at(i*4 + 2) = pixel;
        // alpha channel is always 255
    }
}

void Renderer::draw(sf::RenderWindow &window) const{
    window.draw(screenSprite);
}