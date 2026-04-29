#include "emulation/emulator.hpp"
#include "timing/timing.hpp"

using namespace std;

Emulator::Emulator(sf::Time CPUInstructionTime, sf::Time CPUTimerTime):
    CPUInstructionTime(CPUInstructionTime),
    CPUTimerTime(CPUTimerTime),
    ram(),
    display(64, 32),
    controller(),
    loopTime(),
    paused(true)
{
    cpu = new CPU(ram, display, controller, false);
}

void Emulator::processTime(){
    if(paused) return;

    sf::Time dt = loopTime.restart();
    instructionTimeAccumulator += dt;
    timerTimeAccumulator += dt;

    while(instructionTimeAccumulator > CPUInstructionTime){
        cpu->step();
        instructionTimeAccumulator -= CPUInstructionTime;
    }

    while(timerTimeAccumulator > CPUTimerTime){
        cpu->decTimers();
        timerTimeAccumulator -= CPUTimerTime;
    }
}
vector<pixel_t> Emulator::getDisplayPixels() const{
    return display.getPixels();
}