#include <iostream>
#include <SFML/Graphics.hpp>
#include <CHIP_8/CPU.hpp>
#include <CHIP_8/RAM.hpp>
#include <CHIP_8/controller.hpp>
#include <CHIP_8/display.hpp>
#include <emulation/emulator.hpp>
#include <emulation/renderer.hpp>
#include <filesystem>

int main(int argc, char** argv)
{
    RAM ram = RAM();
    Display display = Display(64, 32);
    Controller controller = Controller();
    CPU cpu(ram, display, controller, false);

    Emulator emulator = Emulator(cpu, ram, display, controller, T500Hz, T60Hz);
    Renderer renderer = Renderer(64, 32, 10.0f);

    sf::RenderWindow window(sf::VideoMode(640,320), "JC8E");
    sf::Clock dt;
    dt.restart();

    // TODO Implement a real way to load a rom
    std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/1-chip8-logo.ch8";
    emulator.load(romPath);
    emulator.unpause();

    while(window.isOpen()) {
        emulator.processTime(dt.restart());
        
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