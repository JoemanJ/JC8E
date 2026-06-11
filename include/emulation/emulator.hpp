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
        
        If ignorePaused is false (default) this function will not process the
        elapsed time if the emulator is paused, effectively freezing the 
        emulator time
        If ignorePaused is true, this function will account for the given time
        even if the emulator is paused, which is useful if you want to execute
        one instruction or timer decrease at a time for debugging purposes.
        */
        void processTime(const sf::Time& dt, bool ignorePaused = false);

        /*
        Returns an array of 64*32 bytes representing each pixel on the display.
        An "off" has value 0; an "on" pixel has value 255.
        */
        const std::vector<pixel_t>& getDisplayPixels() const;

        // Pauses emulation
        void pause() {paused = true;}

        // Resumes emulation
        void unpause() {paused = false;}

        // Executes exactly one CPU instruction (does not affect timers)
        void step() {cpu->step();}

        // Process the exact time of a CPU instruction execution (decreases CPU timers accordingly)
        void stepInstructionTime(){processTime(CPUInstructionTime, true);}

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

        // Returns a pointer to the CPU's general purpose registers (V0-VF) raw data
        byte_t* getCPURegs(){return cpu->getRegs().data();}

        // Returns a pointer to the CPU's Program Counter register
        addr_t* getCPUPC(){return &cpu->getPC();}

        // TODO: Figure aut a way to expose the stack data
        // Returns a pointer to the CPU's address stack's raw data
        // addr_t* getStack(){return cpu->getStack();}

        // Returns a pointer to the CPU's Index register
        addr_t* getCPUI(){return &cpu->getI();}

        // Returns a pointer to the CPU's Delay Timer register
        byte_t* getCPUDelayTimer(){return &cpu->getDelayTimer();}

        // Returns a pointer to the CPU's Sound Timer register
        byte_t* getCPUSoundTimer(){return &cpu->getSoundTimer();}

        // Resets the currently loaded rom
        void reset();

    private:
        sptr<IRAM> ram;
        sptr<IDisplay> display;
        sptr<IController> controller;
        uptr<ICPU> cpu;

        std::filesystem::path loadedRomPath = "";
        
        bool paused;

        sf::Time CPUInstructionTime;
        sf::Time CPUTimerTime;
        
        sf::Time instructionTimeAccumulator;
        sf::Time timerTimeAccumulator;
    };