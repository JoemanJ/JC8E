#pragma once
#include <array>
#include <stack>
#include "commons.hpp"
#include "IRAM.hpp"
#include "Idisplay.hpp"

// Forward declaration in case we don't compile with googleTest
class CPUTest;

// Standart CHIP 8 font
inline constexpr byte_t FONT[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
}; 

class CPU{
    // For unit tests
    friend class CPUTest;

    public:
        /* Creates a new CPU with:
        memory:  standard font from 0x050 to 0x09F, al other addresses are 0x00
        regs: all 0
        PC: 0x200
        I: 0*/
        CPU(IRAM& ram, IDisplay& display);
        
        byte_t memRead (addr_t address) const; // Read byte from RAM address.
        
        /* Reads value from a general purpose register.
        Valid values for reg are 0 to 15. */
        byte_t regRead(addr_t reg) const;
        /* Reads value from a general purpose register.
        Valid values for reg are '0' to '9' and 'A' to 'F' */
        byte_t regRead(char reg) const;
        
        addr_t IRead() const; // Reads value from index register.
        
        addr_t PCRead() const {return PC;} // Reads value from PC register.
        
    private:
        IRAM& memory; // RAM Memory.
        IDisplay& display; // Display with binary pixel resolution
        std::array<byte_t, 16> regs; // General purpose registers.
        std::stack<addr_t> stack; // Address stack.
        addr_t PC; // Program Counter register.
        addr_t I; // Index Register.
        // byte_t delay_timer; // Delay Timer.
        // byte_t sound_timer; // Sound Timer.
        
        void stackPush(addr_t address); // Pushes value onto stack.
        addr_t stackPop(); // Pops value from stack.
        
        void memWrite(addr_t address, byte_t value); // Write byte do RAM.
        
        /* Writes value to a general purpose register.
        Valid values for reg are 0 to 15. */
        void regWrite(uint8_t reg, byte_t value);
        /* Writes value to a general purpose register.
        Valid values for reg are '0' to '9' and 'A' to 'F'. */
        void regWrite(char reg, byte_t value);
        
        void IWrite(addr_t value); // Writes value to index register.

        /* Fetches next 16-bit instruction (from PC and PC+1) and updates PC */
        instruction_t fetch();

        // Decodes and executes given instruction
        void decode_execute(instruction_t instruction);
};