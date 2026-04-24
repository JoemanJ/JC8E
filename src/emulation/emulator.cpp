#include "emulation/emulator.hpp"
#include "timing/timing.hpp"

Emulator::Emulator(
    CPU& cpu, RAM& ram, Display& display,
    sf::Time CPUInstructionTime,
    sf::Time CPUTimerTime):
    cpu(cpu), ram(ram), display(display)
{
    CPUInstructionTime = CPUInstructionTime;
    CPUTimerTime = CPUTimerTime;
    loopTime = sf::Clock();
}

void Emulator::processTime(){
    sf::Time dt = loopTime.restart();
    instructionTimeAccumulator += dt;
    timerTimeAccumulator += dt;

    while(instructionTimeAccumulator > CPUInstructionTime){
        cpu.step();
        instructionTimeAccumulator -= CPUInstructionTime;
    }

    while(timerTimeAccumulator > CPUTimerTime){
        cpu.decTimers();
        timerTimeAccumulator -= CPUTimerTime;
    }
}