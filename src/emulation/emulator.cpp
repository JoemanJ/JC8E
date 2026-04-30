#include "emulation/emulator.hpp"
#include "timing/timing.hpp"

using namespace std;

Emulator::Emulator(sf::Time CPUInstructionTime, sf::Time CPUTimerTime):
    CPUInstructionTime(CPUInstructionTime),
    CPUTimerTime(CPUTimerTime),
    ram(),
    cpu(ram, display, controller, false),
    display(64, 32),
    controller(),
    paused(true)
{
}

void Emulator::processTime(const sf::Time& dt){
    if(paused) return;

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
vector<pixel_t> Emulator::getDisplayPixels() const{
    return display.getPixels();
}

void Emulator::load(const std::filesystem::path &path){
    ram.load(path);
    unpause();
}