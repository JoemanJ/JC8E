#include <emulation/emulator.hpp>
#include <emulation/inputMapper.hpp>
#include <SFML/Graphics.hpp>
#include <commons.hpp>

struct Config{
    struct{
        struct{
            bool resettingUI = true;

            bool showDisplay = true;
            bool showMemory = true;
            bool showRegisters = true;
            bool showExecutionControls = true;

            bool forceUniformScale = true;
        } rendering;

        struct{
            bool RAMEditorAlreadyCreated = false;
        } state;
    } runtime;
};

/*
    Class responsible for interfacing the emulator environment with the operating system. This 
    includes keeping track of time, rendering the GUI and handling system events. 
*/
class Application{
    public:
        Application();
        ~Application() = default;

        // Starts the emulator and calls the mainLoop function until the application needs to close
        // (see CLOSING attribute)
        void run();
        
    private:
        /*
            Initializes a default original CHIP_8 emulator. Initializes RAM, Display, Controller and
            CPU internally.
        */
        void initEmulator();
        uptr<Emulator> emulator;

        /*
        Creates a SFML window and attaches a Dear ImGui context to it.
        */
        void initWindow();
        sf::RenderWindow window;

        InputMapper inputMapper;
        sf::Clock loopClock;
        
        sf::Texture gameTexture; // game display texture
        sf::Sprite gameSprite; // game display sprite

        // Auxiliary buffer for converting pixels from CHIP 8 format to SFML's RGB 
        std::vector<sf::Uint8> pixelBuffer;

        struct Config config;

        // Signals that the user wants to close the application (e.g. clicked to close the window)
        bool CLOSING = false;

        /*
            The main loop function does the following every iteration:
            - Checks for system events (key presses, clicks, etc);
                - Passes these events to Dear ImGui;
                - Processes each event accordingly;
            - Calls emulator.process() with the elapsed time;
            - Renders a new frame;
                - Draws the main Dear ImGui docking space;
                - Draws each Dear ImGui componnent.
        */
        void mainLoop();

        /*
            Handles system events and passes them to Dear ImGui.
        */
        void handleEvents();

        // Renders a single UI frame, including game display and all DearImGui Widgets
        // We pass a time here because imgui-SFML needs the loop time to update the UI
        void renderFrame(sf::Time dt);

        // Creates a Dear ImGui dockspace to house all windows
        void createMainDockSpace();

        // Renders the game display
        void renderDisplay();

        // Renders the memory viewer/hex editor
        void renderMemory();
        
        // Renders the registers display
        void renderRegisters();

        // Renders the execution controls
        void renderExecutionControls();

        // Updates gameSprite with the data from the emulator display. Automatically converts pixels
        // From CHIP 8's black and white format to SFML's RGB 
        void updateGameSprite();
        
};