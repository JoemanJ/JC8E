#include <gtest/gtest.h>
#include <array>
#include "CHIP_8/CPU.hpp"
#include "commons.hpp"

using namespace std;

class CPUTest : public testing::Test {
    protected:
        CPU cpu;

        // Auxiliary functions to access private members
        const array<byte_t, RAM_SIZE>& getMemory(){return cpu.memory;}
        const array<byte_t, 16>& getRegs(){return cpu.regs;}
        const addr_t& getPC(){return cpu.PC;}
        const addr_t& getI(){return cpu.I;}
};

TEST_F(CPUTest, AllGeneralPurposeRegistersStart0x00){
    for(byte_t r : getRegs()){
        EXPECT_EQ(r, 0x00);
    }
}

TEST_F(CPUTest, ProgramCounterStartsWith0x200){
    EXPECT_EQ(getPC(), 0x200);
}

TEST_F(CPUTest, IRegisterStartsWith0x0000){
    EXPECT_EQ(getI(), 0x00);
}

TEST_F(CPUTest, RAMIsAllZeroesExceptFromStandardFontFrom0x50To0x9F){    
    auto memory = getMemory();
    addr_t i = 0;
    for(; i<0x50; i++){
        EXPECT_EQ(memory.at(i), 0x00);
    }
    for(i=0x50; i<0x9F; i++){
        EXPECT_EQ(memory.at(i), FONT[i]);
    }
    for(i=0xA0; i<memory.size(); i++){
        EXPECT_EQ(memory.at(i), 0x00);
    }
}