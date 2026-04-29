#include <iostream>
#include <SFML/Graphics.hpp>
#include <CHIP_8/CPU.hpp>
#include <CHIP_8/RAM.hpp>
#include <CHIP_8/controller.hpp>
#include <CHIP_8/display.hpp>
#include <emulation/emulator.hpp>
#include <emulation/renderer.hpp>

int main(int argc, char** argv)
{
    Emulator emulator = Emulator();
    Renderer renderer = Renderer(64, 32, 10.0f);

    sf::RenderWindow window(sf::VideoMode(640,320), "JC8E");

    while(window.isOpen()) {
        emulator.processTime();
        
        // Event handling
        sf::Event event;
        while(window.pollEvent(event)){
            // Close window
            if (event.type == sf::Event::Closed) window.close();
        }

        // Frame pipeline
        window.clear(sf::Color::Black);
        // Draw stuff...
        renderer.update(emulator.getDisplayPixels());
        renderer.draw(window);
        window.display();
    }

    return 0;
}