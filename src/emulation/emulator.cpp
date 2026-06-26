#include "emulation/emulator.hpp"

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
    CPUInstructionTime(CPUInstructionTime), CPUTimerTime(CPUTimerTime), paused(true),
    breakpoints()
{
      
}

void Emulator::step(){
    cpu->step();
}

inline bool Emulator::isBreakpoint(addr_t address) const{
    for(auto it = breakpoints.begin(); it!=breakpoints.end(); it++){
        if (*it == address) return true;
    }

    return false;
}
void Emulator::processTime(const sf::Time &dt, bool ignorePaused)
{
    if(!ignorePaused && paused) return;

    instructionTimeAccumulator += dt;
    timerTimeAccumulator += dt;

    while(instructionTimeAccumulator >= CPUInstructionTime){
        cpu->step();
        instructionTimeAccumulator -= CPUInstructionTime;
        if(isBreakpoint(cpu->getPC())){
            pause();
            return;
        };
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
    cpu->reset();
}