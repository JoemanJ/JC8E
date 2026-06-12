#include <CHIP_8/RAM.hpp>
#include <CHIP_8/display.hpp>
#include <CHIP_8/controller.hpp>
#include <CHIP_8/CPU.hpp>
#include <application.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_internal.h> // For building default dockspace layout
#include <imgui_memory_editor.h>
#include <imfilebrowser.h>

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
    window.create(sf::VideoMode::getDesktopMode(), "JC8E", sf::Style::Default);
    if (!igsf::Init(window)) return;
    ig::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Application::run(){
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

    createMenuBar();
    createMainDockSpace();
    renderDisplay();
    renderMemory();
    renderRegisters();
    renderExecutionControls();
    
    igsf::Render();
    window.display();
}

void Application::createMenuBar(){
    ig::BeginMainMenuBar();

    // File menu
    static ig::FileBrowser fileBrowser;
    if(ig::BeginMenu("File")){
        if(ig::MenuItem("Open ROM", "Ctrl+O")){
            if(!config.runtime.state.loadROMBrowserAlreadyCreated){
                fileBrowser.SetTitle("Select ROM file to load");
                fileBrowser.SetTypeFilters({".ch8", ".bin", ".*"});
            }
    
            fileBrowser.Open();
        }
        ig::EndMenu();
    }
    fileBrowser.Display();
    if(fileBrowser.HasSelected()){
        // TODO: Change this to a try-catch with an error message modal
        // TODO: Change this to a method that also adds the rom to the recent roms list
        emulator->load(fileBrowser.GetSelected());
        config.runtime.state.romIsLoaded = true;
        fileBrowser.ClearSelected();

    }

    // View menu
    if(ig::BeginMenu("View")){
        ig::MenuItem("Display", NULL, &config.runtime.rendering.showDisplay);
        ig::MenuItem("Registers", NULL, &config.runtime.rendering.showRegisters);
        ig::MenuItem("Execution controls", NULL, &config.runtime.rendering.showExecutionControls);
        ig::MenuItem("Memory editor", NULL, &config.runtime.rendering.showMemory);
    ig::EndMenu();
    }

    ig::EndMainMenuBar();
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
        ig::SeparatorText("General Purpose Registers:");
        uint8_t registerIndex = 0; // Counting from 0 to F
        if(ig::BeginTable("General Purpose Registers", 4)){
            for(uint8_t row=0; row<4; row++){
                ig::TableNextRow();
                for(uint8_t col=0; col<4; col++){
                    // FIXME?: Maybe there's a better way to do this...
                    // Generating labels "R0" to "RF"
                    char regName[5] = "##RX";
                    sprintf(&regName[3], "%01X", registerIndex);

                    ig::TableSetColumnIndex(col);
                    ig::Text("R%01X", registerIndex);
                    ig::InputScalar(regName, ImGuiDataType_U8, 
                        &regs[registerIndex], NULL, NULL, "%02X");
                    registerIndex++;
                }
            }
            ig::EndTable();
        }

        ig::Text("PC: "); ig::SameLine(); ig::Text("%d", *emulator->getCPUPC());
        ig::Text("I: "); ig::SameLine(); ig::Text("%d", *emulator->getCPUI());
        ig::Text("Delay Timer: "); ig::SameLine(); ig::Text("%d", *emulator->getCPUDelayTimer());
        ig::Text("Sound Timer: "); ig::SameLine(); ig::Text("%d", *emulator->getCPUSoundTimer());
    ig::End();
}

void Application::renderExecutionControls(){
    if(!config.runtime.rendering.showExecutionControls) return;
    bool romIsLoaded = config.runtime.state.romIsLoaded;
    
    ig::Begin("Execution Controls", &config.runtime.rendering.showExecutionControls);
        ig::BeginDisabled(!romIsLoaded);
            if(ig::Button("Play")) emulator->unpause();
            ig::SameLine();
            if(ig::Button("Pause")) emulator->pause();
            ig::SameLine();
            if(ig::Button("Step")) emulator->stepInstructionTime();
            ig::SameLine();
            if(ig::Button("Restart")) emulator->reset();
        ig::EndDisabled();
        
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