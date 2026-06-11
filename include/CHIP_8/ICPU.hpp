#pragma once
#include <commons.hpp>
#include <stack>

class ICPU{
    public:
        virtual ~ICPU() = default;
        virtual void step() = 0;
        virtual void decTimers() = 0;
        
        // Returns a reference to the general purpose registers (V0-VF)
        virtual std::array<byte_t, 16>& getRegs() = 0;

        // Returns a reference to the Program Counter register
        virtual addr_t& getPC() = 0;

        // Returns a reference to the address stack
        virtual std::stack<addr_t>& getStack() = 0;

        // Returns a reference to the Index register
        virtual addr_t& getI() = 0;

        // Returns a reference to the Delay Timer register
        virtual byte_t& getDelayTimer() = 0;

        // Returns a reference to the Sound Timer register
        virtual byte_t& getSoundTimer() = 0;

        /* 
        Resets CPU to initial state and writes default font to RAM, starting from
        address 0x50.
        
        Initial state:
        - All general purpose registers (R0 to RF) = 0
        - Index register (I) = 0
        - Program Counter register (PC) = 0x200
        - Sound timer register = 0
        - Delay timer register = 0
        */
        virtual void reset() = 0;
};