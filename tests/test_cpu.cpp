#include <gtest/gtest.h>
#include <array>
#include "CHIP_8/CPU.hpp"
#include "CHIP_8/RAM.hpp"
#include "commons.hpp"

using namespace std;

class CPUTest : public testing::Test {
    protected:
        RAM ram;
        CPU cpu;
        // Auxiliary functions to access private members
        const array<byte_t, 16>& getRegs(){return cpu.regs;}
        const addr_t& getPC(){return cpu.PC;}
        const addr_t& getI(){return cpu.I;}
    
    public:
    CPUTest(): ram(), cpu(ram){}
};

TEST_F(CPUTest, AllGeneralPurposeRegistersStartWith0x00){
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

TEST_F(CPUTest, RAMIsAllZeroesUpTo0x50){    
    addr_t i = 0;
    for(; i<0x50; i++){
        EXPECT_EQ(cpu.memRead(i), 0x00);
    }
}

TEST_F(CPUTest, RAMHasStandardFontFrom0x50To0x9F){
    addr_t i = 0x50;
    for (i=0x50; i<=0x9F; i++){
        EXPECT_EQ(cpu.memRead(i), FONT[i-0x50]);
    }
}

TEST_F(CPUTest, RAMIsAllZerosFromWhereTheStandardFontEndsOn0x9FOnward){
    addr_t i = 0xA0;
    for (i=0xA0; i<RAM_SIZE; i++){
        EXPECT_EQ(cpu.memRead(i), 0x00);
    }
}