#include "emulation/emulator.hpp"
#include "timing/timing.hpp"

using namespace std;

Emulator::Emulator(
    uptr<ICPU> cpu, 
    sptr<IRAM> ram, 
    sptr<IDisplay> display, 
    sptr<IController> controller, 
    sf::Time CPUInstructionTime, 
    sf::Time CPUTimerTime
): 
    cpu(std::move(cpu)), ram(ram), display(display), controller(controller),
    CPUInstructionTime(CPUInstructionTime), CPUTimerTime(CPUTimerTime), paused(true)
{
      
}

void Emulator::processTime(const sf::Time &dt, bool ignorePaused)
{
    if(!ignorePaused && paused) return;

    instructionTimeAccumulator += dt;
    timerTimeAccumulator += dt;

    while(instructionTimeAccumulator >= CPUInstructionTime){
        cpu->step();
        instructionTimeAccumulator -= CPUInstructionTime;
    }

    while(timerTimeAccumulator >= CPUTimerTime){
        cpu->decTimers();
        timerTimeAccumulator -= CPUTimerTime;
    }
}
const vector<pixel_t>& Emulator::getDisplayPixels() const{
    return display->getPixels();
}

void Emulator::pressKey(const byte_t k){
    controller->press(k);
}

void Emulator::releaseKey(const byte_t k){
    controller->release(k);
}

void Emulator::load(const std::filesystem::path &path){
    controller->reset();
    display->reset();
    ram->load(path);
    cpu->reset();
    loadedRomPath = path;
    unpause();
}

byte_t* Emulator::getRAMBytes(){
    return ram->getRawMemory();
}

void Emulator::reset(){
    controller->reset();
    display->reset();
    ram->load(loadedRomPath);
    cpu.reset();
}