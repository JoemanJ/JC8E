#include <iostream>
#include <SFML/Graphics.hpp>
#include <CHIP_8/CPU.hpp>
#include <CHIP_8/RAM.hpp>
#include <CHIP_8/controller.hpp>
#include <CHIP_8/display.hpp>
#include <emulation/emulator.hpp>

int main(int argc, char** argv)
{
    // Setup CPU
    RAM ram = RAM();
    Display display = Display(SCREEN_WIDTH, SCREEN_HEIGHT);
    Controller controller = Controller();
    CPU cpu = CPU(ram, display, controller);

    Emulator emulator(cpu, ram, display);

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
        window.display();
    }

    return 0;
}