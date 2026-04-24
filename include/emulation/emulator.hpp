#pragma once

#include "CHIP_8/CPU.hpp"
#include "CHIP_8/RAM.hpp"
#include "CHIP_8/display.hpp"
#include "timing/timing.hpp"

class Emulator {
    public:
        CPU& cpu;
        RAM& ram;
        Display& display;

        Emulator(
            CPU& cpu, RAM& ram, Display& display,
            sf::Time CPUInstructionTime = T500Hz,
            sf::Time CPUTimerTime = T60Hz
        );

        /*
        This function should be called in the main loop.
        This will run CPU instructions and decrement CPU timers according to
        elapsed time since the last iteration.
        */
        void processTime();

    private:
        sf::Clock loopTime;
        sf::Time CPUInstructionTime;
        sf::Time CPUTimerTime;

        sf::Time instructionTimeAccumulator;
        sf::Time timerTimeAccumulator;
};