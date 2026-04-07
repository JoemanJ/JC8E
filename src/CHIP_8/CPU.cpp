#include <stdexcept>
#include <sstream>
#include <iomanip>
#include "CHIP_8/CPU.hpp"

using namespace std;
CPU::CPU(IRAM& ram, IDisplay& display): memory(ram), display(display){
    // Copy font to memory
    memory.bulkWrite(0x50, sizeof(FONT), FONT);
    
    // Initialize registers
    regs = std::array<byte_t, 16>();
    PC = 0x200;
    I = 0x0;
}

byte_t CPU::memRead(addr_t address) const{
    return memory.read(address);
}

void CPU::memWrite(addr_t address, byte_t value){
    memory.write(address, value);
}

void CPU::stackPush(addr_t address){
    stack.push(address);
}

addr_t CPU::stackPop(){
    addr_t address = stack.top();
    stack.pop();
    return address;
}

uint16_t CPU::fetch()
{
    uint16_t instruction = (memory.read(PC) << 8) | memory.read(PC+1);
    PC += 2;
    return instruction;
}

// Returns an exception saying which invalid instruction tried to execute 
inline invalid_argument invalidInstruction(instruction_t instruction){
    // This will format the instruction to show as "0xABCD"
    stringstream exceptionText;
    exceptionText << "CPU tried to execute invalid instruction: " <<
                        std::hex << std::uppercase << std::showbase <<
                        instruction;

    return invalid_argument(exceptionText.str());
}

inline void CPU::setFlag(){
    regs.at(0xF) = 1;
}

inline void CPU::resetFlag(){
    regs.at(0xF) = 0;
}

void CPU::decode_execute(instruction_t instruction){
    // Separate instruction into pieces
    instruction_t TYPE, X, Y, N, NN, NNN;
    TYPE    = ((instruction & 0xF000) >> 12); // First nibble 
    X       = ((instruction & 0x0F00) >> 8);  // Second nibble
    Y       = ((instruction & 0x00F0) >> 4);  // Third nibble
    N       = ((instruction & 0x000F) >> 0);  // Fourth nibble
    NN      = ((instruction & 0x00FF) >> 0);  // Third and fourth bytes
    NNN     = ((instruction & 0x0FFF) >> 0);  // Second, third and fourth bytes

    switch (TYPE){
        case 0x0:
            switch (NNN){
                case 0x0E0: // 0x00E0 = clear screen
                    display.clear();
                    break;
                
                case 0x0EE: // 0x00EE = return from subroutine
                    PC = stackPop();
                    break;

                default:
                    throw invalidInstruction(instruction);
            }
            break;
        
        case 0x1: // 0x1NNN = Jump to NNN
            PC = NNN;
            break;

        case 0x2: // 0x2NNN = Call subroutine at NNN
            stack.push(PC);
            PC = NNN;
            break;

        case 0x3: // 0x3XNN = Skip instruction if VX = NN
            if (regs.at(X) == NN) PC += 2;
            break;

        case 0x4: // 0x4XNN = Skip instruction if VX !- NN
            if (regs.at(X) != NN) PC += 2;
            break;

        case 0x5:
            switch(N){
                case 0: // 0x5XY0 - Skip instruction if VX == VY
                    if (regs.at(X) == regs.at(Y)) PC += 2;   
                    break;
                
                default:
                throw invalidInstruction(instruction);
            }
            break;

        case 0x6: // 0x6XNN = Set VX to NN
            regs.at(X) = NN;
            break;

        case 0x7: // 0x7XNN = VX = VX + NN. Carry flag is NOT affected
            regs.at(X) += NN;
            break;

        case 0x8: // Logic and arithmetic instructions
            switch(N){
                case 0: // 0x8XY0 Set VX to VY
                    regs.at(X) = regs.at(Y);
                    break;

                case 1: // 0x8XY1 VX = VX OR VY (bitwise)
                    regs.at(X) = regs.at(X) | regs.at(Y);
                    break;

                case 2: // 0x8XY2 VX = VX AND VY (bitwise)
                    regs.at(X) = regs.at(X) & regs.at(Y);
                    break;

                case 3: // 0x8XY3 XY = VX XOR VY (bitwise)
                    regs.at(X) = regs.at(X) ^ regs.at(Y);
                    break;

                case 4: // 0x8XY4 XY = VX + VY. Carry flag IS affected
                {
                    uint16_t result = regs.at(X) + regs.at(Y);
                    if (result > 255) setFlag();
                    else resetFlag();
                    regs.at(X) += regs.at(Y);
                    break;
                }
                
                default:
                    throw invalidInstruction(instruction);
            }
            break;

        case 0x9:
            switch(N){
                case 0: // 0x9XY0 - Skip instruction if VX != VY
                    if (regs.at(X) != regs.at(Y)) PC += 2;   
                    break;
                
                default:
                throw invalidInstruction(instruction);
            }
            break;

        default:
            throw invalidInstruction(instruction);            
    }
}
