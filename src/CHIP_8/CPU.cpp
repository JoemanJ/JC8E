#include <stdexcept>
#include <sstream>
#include <iomanip>
#include "CHIP_8/CPU.hpp"

using namespace std;
CPU::CPU(IRAM& ram, IDisplay& display, IController& controller, bool USE_LEGACY_BEHAVIOR):
memory(ram),
display(display),
controller(controller),
USE_LEGACY_BEHAVIOR(USE_LEGACY_BEHAVIOR)
{
    // Copy font to memory
    memory.bulkWrite(0x50, sizeof(FONT), FONT);
    
    // Initialize registers
    regs = std::array<byte_t, 16>();
    PC = 0x200;
    I = 0x0;

    // Initialize random number generator
    RNG = mt19937(random_device()());
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
        
        case 0x1: // 0x1NNN Jump to NNN
            PC = NNN;
            break;

        case 0x2: // 0x2NNN Call subroutine at NNN
            stack.push(PC);
            PC = NNN;
            break;

        case 0x3: // 0x3XNN Skip instruction if VX = NN
            if (regs.at(X) == NN) PC += 2;
            break;

        case 0x4: // 0x4XNN Skip instruction if VX !- NN
            if (regs.at(X) != NN) PC += 2;
            break;

        case 0x5:
            if (N != 0) throw invalidInstruction(instruction);
            
            // 0x5XY0 Skip instruction if VX == VY
            if (regs.at(X) == regs.at(Y)) PC += 2;
            break;

        case 0x6: // 0x6XNN Set VX to NN
            regs.at(X) = NN;
            break;

        case 0x7: // 0x7XNN VX = VX + NN. Carry flag is NOT affected
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

                case 4: // 0x8XY4 XY = VX + VY. Carry flag set on overflow
                {
                    uint16_t result = regs.at(X) + regs.at(Y);
                    if (result > 255) setFlag();
                    else resetFlag();
                    regs.at(X) += regs.at(Y);
                    break;
                }

                case 5: // 0x8XY5 VX = VX - VY. Carry flag set if underflow doesn't occur
                    if(regs.at(Y) > regs.at(X)) resetFlag();
                    else setFlag();
                    regs.at(X) -= regs.at(Y);
                    break;

                case 6: // 0x8XY6 Shift VX 1 bit to the right
                    if (USE_LEGACY_BEHAVIOR) regs.at(X) = regs.at(Y);
                    if(regs.at(X) & 0b00000001) setFlag();
                    else resetFlag();
                    regs.at(X) = regs.at(X) >> 1;
                    break;

                case 7: // 0x8XY7 VX = VY - VX. Carry flag set if underflow doesn't occur
                    if(regs.at(X) > regs.at(Y)) resetFlag();
                    else setFlag();
                    regs.at(X) = regs.at(Y) - regs.at(X);
                    break;

                case 0xE: //0x8XYE Shift VX 1 bit to the left
                    if (USE_LEGACY_BEHAVIOR) regs.at(X) = regs.at(Y);
                    if(regs.at(X) & 0b10000000) setFlag();
                    else resetFlag();
                    regs.at(X) = regs.at(X) << 1;
                    break;

                default:
                    throw invalidInstruction(instruction);
            }
            break;

        case 0x9:
            if (N != 0) throw invalidInstruction(instruction);

            // 0x9XY0 Skip instruction if VX != VY
            if (regs.at(X) != regs.at(Y)) PC += 2;
            break;

        case 0xA: // 0xANNN Set index register to NNN 
            I = NNN;
            break;

        case 0xB: // 0xBNNN Jump to NNN with offset
            PC = NNN;
            if (USE_LEGACY_BEHAVIOR) PC += regs.at(0); // offset is V0 in legacy behavior
            else PC += regs.at(X); // offset is VX in modern behavior 
            break;

        case 0xC: // 0xCXNN VX = (random number) AND NN (binary AND)
        {
            byte_t random = uniform_int_distribution(0, 255)(RNG);
            regs.at(X) = random & NN;
            break;
        }

        /*
            0xDXYN draw an N pixels tall sprite pointed to by the Index register,
            at coordinates (VX, VY) to the screen.
            1 on the sprite means the pixel is toggled.
            0 on the sprite means the pixel is not toggled.
            The starting value of VX wraps around the screen, the sprite does not.
            if any pixels go from 1 to 0, VF is set, otherwise it is reset.
        */
        case 0xD: 
            {
                byte_t displayHeight = display.getHeight();
                byte_t displayWidth = display.getWidth();
                byte_t startX = regs.at(X) % display.getWidth();
                byte_t startY = regs.at(Y) % display.getHeight();
                resetFlag();

                for(byte_t i = 0; i<N && startY+i < displayHeight; i++){ // for each line in the sprite
                    byte_t spriteLine = memory.read(I+i);
                    // IDISPLAY VAI PRECISAR DE UMA FUNÇÃO GETPIXEL
                    for (byte_t j=0; j<8 && startX+j < displayWidth; j++){
                        if (spriteLine & (0b10000000 >> j)){
                            byte_t x = startX + j;
                            byte_t y = startY + i;
                            if (display.getPixel(x, y)) setFlag(); // collision
                            display.togglePixel(x, y);
                        }
                    }
                }
            }
            break;

        case 0xE:
            switch(NN){
                case 0x9E:
                    if (controller.isPressed(regs.at(X))) PC += 2;
                    break;

                case 0xA1:
                    if (!controller.isPressed(regs.at(X))) PC += 2;
                    break;

                default:
                    throw invalidInstruction(instruction);
            }
            break;

        case 0xF:
            switch(NN){
                case 0x07:
                    regs.at(X) = delayTimer;
                    break;

                case 0x15:
                    delayTimer = regs.at(X);
                    break;

                case 0x18:
                    soundTimer = regs.at(X);
                    break;

                default:
                    throw invalidInstruction(instruction);
                    break;
            }
            break;

        default:
            throw invalidInstruction(instruction);            
    }
}
