#include <iostream>
#include <SFML/Graphics.hpp>
#include <CHIP_8/CPU.hpp>
#include <CHIP_8/RAM.hpp>
#include <CHIP_8/controller.hpp>
#include <CHIP_8/display.hpp>
#include <emulation/emulator.hpp>
#include <emulation/renderer.hpp>
#include <filesystem>
#include <unordered_map>
#include <optional>

using namespace std;

unordered_map<sf::Keyboard::Key, byte_t> keyMapper{
    {sf::Keyboard::Key::Num1, 0x1}, {sf::Keyboard::Key::Num2, 0x2}, {sf::Keyboard::Key::Num3, 0x3}, {sf::Keyboard::Key::Num4, 0xC},
    {sf::Keyboard::Key::Q, 0x4}, {sf::Keyboard::Key::W, 0x5}, {sf::Keyboard::Key::E, 0x6}, {sf::Keyboard::Key::R, 0xD},
    {sf::Keyboard::Key::A, 0x7}, {sf::Keyboard::Key::S, 0x8}, {sf::Keyboard::Key::D, 0x9}, {sf::Keyboard::Key::F, 0xE},
    {sf::Keyboard::Key::Z, 0xA}, {sf::Keyboard::Key::X, 0x0}, {sf::Keyboard::Key::C, 0xB}, {sf::Keyboard::Key::V, 0xF}
};

optional<byte_t> translateKeyPress(sf::Keyboard::Key key){
    auto it = keyMapper.find(key);
    if (it != keyMapper.end()){
        return it->second;
    }
    return std::nullopt;
}

int main(int argc, char** argv)
{
    RAM ram = RAM();
    Display display = Display(64, 32);
    Controller controller = Controller();
    CPU cpu(ram, display, controller, true);

    Emulator emulator = Emulator(cpu, ram, display, controller, T500Hz, T60Hz);
    Renderer renderer = Renderer(64, 32, 10.0f);

    sf::RenderWindow window(sf::VideoMode(640,320), "JC8E");
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
                auto chip8Key = translateKeyPress(event.key.code);
                if(chip8Key.has_value()) emulator.pressKey(chip8Key.value());
            }

            // keyReleased
            else if(event.type == sf::Event::KeyReleased){
                auto chip8Key = translateKeyPress(event.key.code);
                if(chip8Key.has_value()) emulator.releaseKey(chip8Key.value());
            }
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