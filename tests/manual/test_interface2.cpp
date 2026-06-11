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
#include <imgui.h>
#include <imgui-SFML.h>
#include <imfilebrowser.h>

using namespace std;
namespace ig = ImGui;
namespace igsf = ImGui::SFML;

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
    ImGui::SFML::Init(window);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    sf::Clock loop_clock;
    loop_clock.restart();

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

    // Testing file browser
    ig::FileBrowser fileBrowser;
    fileBrowser.SetTitle("Test");
    fileBrowser.SetTypeFilters({".ch8", ".bin", ".*"});

    while(window.isOpen()) {
        sf::Time dt = loop_clock.restart();
        
        // Event handling
        sf::Event event;
        while(window.pollEvent(event)){
            igsf::ProcessEvent(window, event);

            // Close window
            if (event.type == sf::Event::Closed){
                window.close();
                igsf::Shutdown();
                return 0;
            }

            // Key pressed
            else if(event.type == sf::Event::KeyPressed){
                KEYS k = inputMapper.translate(event.key.code);
                if(k != NO_KEY) emulator.pressKey(k);
            }

            // Key released
            else if(event.type == sf::Event::KeyReleased){
                KEYS k = inputMapper.translate(event.key.code);
                if(k != NO_KEY) emulator.releaseKey(k);
            }
        }

        emulator.processTime(dt);
        
        // Frame pipeline
        // ImGui pipeline
        igsf::Update(window, dt);
        
        ig::Begin("Game");
        ig::End();
        
        // SFML pipeline
        window.clear(sf::Color::Black);
        window.pushGLStates();

        
        // Redraw the game screen only if something has changed
        // if(emulator.displayNeedsRedraw()){
            renderer.update(emulator.getDisplayPixels());
            emulator.setDisplayAsUpdated();
            ig::Begin("Game");
                ig::Image(renderer.getScreenSprite());
            ig::End();

            ig::Begin("Feature test");
                if(ig::Button("Open file browser")){
                    fileBrowser.Open();
                }
            ig::End();

            fileBrowser.Display();

            if(fileBrowser.HasSelected()){
                cout << "File '" << fileBrowser.GetSelected() <<"' was selected" << endl;
                fileBrowser.ClearSelected();
            }
        // }
        
        window.popGLStates();
        // window.resetGLStates();
        igsf::Render(); 
        window.display();
    }

    return 0;
}