#pragma once

#include "CHIP_8/CPU.hpp"
#include "CHIP_8/RAM.hpp"
#include "CHIP_8/display.hpp"
#include "CHIP_8/controller.hpp"
#include "timing/timing.hpp"

/*
    Class that encapsulates the emulator core and execution logic.
*/
class Emulator {
    friend class EmulatorTest;
    public:
    
        /*
        Orchestrates the emulation process and interaction between device classes
        through a simple interface.
        */
        Emulator(
            uptr<ICPU> cpu,
            sptr<IRAM> ram,
            sptr<IDisplay> display,
            sptr<IController> controller,

            sf::Time CPUInstructionTime = T500Hz,
            sf::Time CPUTimerTime = T60Hz
       );

        /*
        This function should be called in the main loop.
        This will run CPU instructions and decrement CPU timers according to
        elapsed time since the last iteration.
        */
        void processTime(const sf::Time& dt);

        /*
        Returns an array of 64*32 bytes representing each pixel on the display.
        An "off" has value 0; an "on" pixel has value 255.
        */
        const std::vector<pixel_t>& getDisplayPixels() const;

        // Pauses emulation
        void pause() {paused = true;}

        // Resumes emulation
        void unpause() {paused = false;}

        // Presses a controller key. k must be a byte from 0x0 to 0xF
        void pressKey(const byte_t k);

        // Releases a controller key. k must be a byte from 0x0 to 0xF
        void releaseKey(const byte_t k);

        // Loads a rom from the given path
        void load(const std::filesystem::path& path);

        // Returns true if the display has been updated and needs to be redrawn
        bool displayNeedsRedraw() const {return display->getUpdatedFlag();}

        // Sets the display as updated so it down't neet to be redrawn until the next change
        void setDisplayAsUpdated(){display->resetUpdatedFlag();}

        // Exposes the RAM memory data
        byte_t* getRAMBytes();

    private:
        sptr<IRAM> ram;
        sptr<IDisplay> display;
        sptr<IController> controller;
        uptr<ICPU> cpu;
        
        bool paused;

        sf::Time CPUInstructionTime;
        sf::Time CPUTimerTime;
        
        sf::Time instructionTimeAccumulator;
        sf::Time timerTimeAccumulator;
    };