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
        Initializes the emulator with a new CPU, RAM, Controller, Display and
        timing logic.
        */
        Emulator(
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
        std::vector<pixel_t> getDisplayPixels() const;

        // Pauses emulation
        void pause() {paused = true;}

        // Resumes emulation
        void unpause() {paused = false;}

        // Loads a rom from the given path
        virtual void load(const std::filesystem::path& path);

    private:
        RAM ram;
        Display display;
        Controller controller;
        CPU cpu;
        
        bool paused;

        sf::Time CPUInstructionTime;
        sf::Time CPUTimerTime;
        
        sf::Time instructionTimeAccumulator;
        sf::Time timerTimeAccumulator;
    };