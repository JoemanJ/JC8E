#include <iostream>
#include <SFML/Graphics.hpp>
#include <CHIP_8/CPU.hpp>
#include <CHIP_8/RAM.hpp>
#include <CHIP_8/controller.hpp>
#include <CHIP_8/display.hpp>
#include <emulation/emulator.hpp>
#include <emulation/renderer.hpp>
#include <emulation/inputMapper.hpp>
#include <filesystem>
#include <unordered_map>
#include <optional>

using namespace std;

int main(int argc, char** argv)
{
    sptr<RAM> ram = make_shared<RAM>();
    sptr<Display> display = make_shared<Display>(64, 32);
    sptr<Controller> controller = make_shared<Controller>();
    uptr<CPU> cpu = make_unique<CPU>(ram, display, controller, true);

    Emulator emulator = Emulator(move(cpu), ram, display, controller, T500Hz, T60Hz);
    Renderer renderer = Renderer(64, 32, 10.0f);
    InputMapper inputMapper = InputMapper();

    sf::RenderWindow window(sf::VideoMode(640,320), "JC8E");
    window.setFramerateLimit(30);
    sf::Clock dt;
    dt.restart();

    // TODO Implement a real way to load a rom
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/1-chip8-logo.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/2-ibm-logo.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/3-corax+.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/4-flags.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/5-quirks.ch8";
    std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/6-keypad.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/7-beep.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/8-scrolling.ch8";
    emulator.load(romPath);
    emulator.unpause();

    while(window.isOpen()) {
        emulator.processTime(dt.restart());
        
        // Event handling
        sf::Event event;
        while(window.pollEvent(event)){
            // Close window
            if (event.type == sf::Event::Closed) window.close();
            
            // Key pressed
            else if(event.type == sf::Event::KeyPressed){
                KEYS k = inputMapper.translate(event.key.code);
                if(k != NO_KEY) emulator.pressKey(k);
            }

            // keyReleased
            else if(event.type == sf::Event::KeyReleased){
                KEYS k = inputMapper.translate(event.key.code);
                if(k != NO_KEY) emulator.releaseKey(k);
            }
        }

        // Frame pipeline
        window.clear(sf::Color::Black);
        
        // Redraw the screen only if something has changed
        if(emulator.displayNeedsRedraw()){
            renderer.update(emulator.getDisplayPixels());
            emulator.setDisplayAsUpdated();
            renderer.draw(window);
            window.display();
        } 
    }

    return 0;
}