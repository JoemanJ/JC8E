#include <CHIP_8/RAM.hpp>
#include <CHIP_8/display.hpp>
#include <CHIP_8/controller.hpp>
#include <CHIP_8/CPU.hpp>
#include <application.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_internal.h> // For building default dockspace layout
#include <imgui_memory_editor.h>

using namespace std;
namespace ig = ImGui;
namespace igsf = ImGui::SFML;

Application::Application(){
    initEmulator();
    initWindow();
    
    inputMapper = InputMapper();

    loopClock = sf::Clock();
    loopClock.restart();

    gameTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
    gameSprite.setTexture(gameTexture);

    pixelBuffer = std::vector<sf::Uint8>();
    // We fill all the pixels with 255 so we never have to worry about alpha channel
    for (uint16_t i = 0; i < SCREEN_SIZE*4; i++)
    {
        pixelBuffer.push_back(255);
    }
    
}

void Application::initEmulator(){
    sptr<RAM> ram = make_shared<RAM>();
    sptr<Display> display = make_shared<Display>(SCREEN_WIDTH, SCREEN_HEIGHT);
    sptr<Controller> controller = make_shared<Controller>();
    uptr<CPU> cpu = make_unique<CPU>(ram, display, controller, true);
    
    emulator = make_unique<Emulator>(move(cpu), ram, display, controller, T500Hz, T500Hz);
}

void Application::initWindow(){
    // window.create(sf::VideoMode(640, 320), "JC8E");
    window.create(sf::VideoMode::getDesktopMode(), "JC8E", sf::Style::Default);
    if (!igsf::Init(window)) return;
    ig::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Application::run(){
    //TODO: Implement a real way to load a rom...
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/1-chip8-logo.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/2-ibm-logo.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/3-corax+.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/4-flags.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/5-quirks.ch8";
    std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/6-keypad.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/7-beep.ch8";
    // std::filesystem::path romPath = "/home/ddbrandao/Desktop/Estudos/JC8E/tests/ROMS/8-scrolling.ch8";
    emulator->load(romPath);
    emulator->unpause();

    while(window.isOpen() && !CLOSING){
        mainLoop();
    }
}

void Application::mainLoop(){
    static sf::Time dt = loopClock.restart();

    handleEvents();
    if (CLOSING) return;

    emulator->processTime(dt);
    renderFrame(dt);
}

void Application::handleEvents(){
    static sf::Event event;
    while(window.pollEvent(event)){
        igsf::ProcessEvent(window, event);

         // Close window
        if (event.type == sf::Event::Closed){
            window.close();
            igsf::Shutdown();
            CLOSING = true;
            return;
        }

        // Key pressed
        else if(event.type == sf::Event::KeyPressed){
            KEYS k = inputMapper.translate(event.key.code);
            if(k != NO_KEY) emulator->pressKey(k);
        }

        // Key released
        else if(event.type == sf::Event::KeyReleased){
            KEYS k = inputMapper.translate(event.key.code);
            if(k != NO_KEY) emulator->releaseKey(k);
        }
    }
}

void Application::renderFrame(sf::Time dt){
    igsf::Update(window, dt);

    createMainDockSpace();
    renderDisplay();
    renderMemory();
    renderRegisters();
    renderExecutionControls();
    
    igsf::Render();
    window.display();
}

void Application::createMainDockSpace(){
    ImGuiID dockspaceId = ig::DockSpaceOverViewport(0, ig::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    if(config.runtime.rendering.resettingUI){
        config.runtime.rendering.resettingUI = false;

        // Reset the main docking node
        ig::DockBuilderRemoveNode(dockspaceId);
        ig::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ig::DockBuilderSetNodeSize(dockspaceId, ig::GetMainViewport()->Size); // Ocupy entire window

        // Separate main window layout into areas
        ImGuiID background = dockspaceId;
        ImGuiID main = ig::DockBuilderAddNode(background);
        ImGuiID bottom = ig::DockBuilderSplitNode(main, ImGuiDir_Down, 2.0/9.0, nullptr, &main);
        ImGuiID right = ig::DockBuilderSplitNode(main, ImGuiDir_Right, 0.21, nullptr, &main);
        ImGuiID right_bottom = ig::DockBuilderSplitNode(right, ImGuiDir_Down, 0.2f, nullptr, &right);

        // Set which window will be displayed in each area
        ig::DockBuilderDockWindow("Display", main);
        ig::DockBuilderDockWindow("Memory", bottom);
        ig::DockBuilderDockWindow("Registers", right);
        ig::DockBuilderDockWindow("Execution Controls", right_bottom);

        ig::DockBuilderFinish(dockspaceId);
    }
}

void Application::renderDisplay(){
    if(!config.runtime.rendering.showDisplay) return;

    // Redraw game screen
    if(emulator->displayNeedsRedraw()){
        updateGameSprite();
        emulator->setDisplayAsUpdated();
    }
    
    ig::Begin("Display", &config.runtime.rendering.showDisplay);
        // Scale game texture to fit window size
        ImVec2 area = ImGui::GetContentRegionAvail();
        sf::Vector2u textureSize = gameTexture.getSize();

        float scaleX = area.x / textureSize.x; 
        float scaleY = area.y / textureSize.y;

        if (config.runtime.rendering.forceUniformScale){
            if (scaleX < scaleY) scaleY = scaleX;
            else scaleX = scaleY;
        }
        int totalSizeX = textureSize.x * scaleX;
        int totalSizeY = textureSize.y * scaleY;

        gameSprite.setScale(sf::Vector2f(scaleX, scaleY));
        
        // Center the sprite
        ig::SetCursorPosX(area.x/2 - totalSizeX/2);
        ig::SetCursorPosY(area.y/2 - totalSizeY/2);

        ig::Image(gameSprite);
    ig::End();
}

void Application::renderMemory(){
    if(!config.runtime.rendering.showMemory) return;
    
    static MemoryEditor memoryEditor;

    if (!config.runtime.state.RAMEditorAlreadyCreated){
        config.runtime.state.RAMEditorAlreadyCreated = true;
        memoryEditor.Cols = 64;
    }

    ig::Begin("Memory", &config.runtime.rendering.showMemory);
        memoryEditor.DrawContents(emulator->getRAMBytes(), RAM_SIZE);
    ig::End();
}

void Application::renderRegisters(){
    if(!config.runtime.rendering.showRegisters) return;
    
    static byte_t* regs = emulator->getCPURegs();
    ig::Begin("Registers", &config.runtime.rendering.showRegisters);
        ig::Text("Registers:");
        ig::Text("%02X %02X %02X %02X\n%02X %02X %02X %02X\n%02X %02X %02X %02X\n%02X %02X %02X %02X\n\n",
        regs[0], regs[1], regs[2], regs[3], 
        regs[4], regs[5], regs[6], regs[7], 
        regs[8], regs[9], regs[10], regs[11],
        regs[12], regs[13], regs[14], regs[15]);

        ig::Text("PC: "); ig::SameLine(); ig::Text("%d", *emulator->getCPUPC());
        ig::Text("I: "); ig::SameLine(); ig::Text("%d", *emulator->getCPUI());
        ig::Text("Delay Timer: "); ig::SameLine(); ig::Text("%d", *emulator->getCPUDelayTimer());
        ig::Text("Sound Timer: "); ig::SameLine(); ig::Text("%d", *emulator->getCPUSoundTimer());
    ig::End();
}

void Application::renderExecutionControls(){
    if(!config.runtime.rendering.showExecutionControls) return;
    
    ig::Begin("Execution Controls", &config.runtime.rendering.showExecutionControls);
        if(ig::Button("Play")) emulator->unpause();
        ig::SameLine();
        if(ig::Button("Pause")) emulator->pause();
        ig::SameLine();
        // TODO: Change this to dinamically account for the emulator's execution time
        if(ig::Button("Step")) emulator->step();
        ig::SameLine();
        // TODO: Implement Restart button
    ig::End();
}

static void convertPixelsFromChip8(const vector<byte_t>& source, vector<sf::Uint8>& target){
    for(uint16_t i=0; i<SCREEN_SIZE; i++){
        byte_t pixel = source.at(i);
        target.at(i*4 + 0) = pixel;
        target.at(i*4 + 1) = pixel;
        target.at(i*4 + 2) = pixel;
        // alpha channel is always 255
    }
}

void Application::updateGameSprite(){
    convertPixelsFromChip8(emulator->getDisplayPixels(), this->pixelBuffer);
    gameTexture.update(pixelBuffer.data());
}