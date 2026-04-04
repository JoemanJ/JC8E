#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <array>
#include "CHIP_8/CPU.hpp"
#include "CHIP_8/RAM.hpp"
#include "CHIP_8/display.hpp"
#include "commons.hpp"

using namespace std;

class MockRAM : public IRAM {
    public:
        MOCK_METHOD(void, write, (addr_t address, byte_t value), (override));
        MOCK_METHOD(byte_t, read, (addr_t address), (const, override));
        MOCK_METHOD(void, bulkWrite, (addr_t startAddress, std::size_t size, const byte_t *data), (override));
};

class MockDisplay : public IDisplay {
    public:
        MOCK_METHOD(void, togglePixel, (byte_t x, byte_t y), (override));
        MOCK_METHOD(void, clear, (), (override));
};

class CPUTest : public testing::Test {
    protected:
        MockRAM ram;
        MockDisplay display;
        CPU cpu;

        // Auxiliary functions to access private members
        array<byte_t, 16>& getRegs(){return cpu.regs;}
        addr_t& getPC(){return cpu.PC;}
        addr_t& getI(){return cpu.I;}
    
    public:
        CPUTest(): ram(), display(), cpu(ram, display){}
        
        // Public functions that expose the CPU's private functions
        void stackPush(){cpu.stackPush();};
        addr_t stackPop(){return cpu.stackPop();};
        void memWrite(addr_t address, byte_t value){cpu.memWrite(address, value);}
        void regWrite(uint8_t reg, byte_t value){cpu.regWrite(reg, value);}
        void regWrite(char reg, byte_t value){cpu.regWrite(reg, value);}
        void IWrite(addr_t value){cpu.IWrite(value);}
        instruction_t fetch(){return cpu.fetch();}
        void decode_execute(instruction_t instruction){cpu.decode_execute(instruction);}
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

TEST_F(CPUTest, CPUCanWriteAndReadFromRAM){
    memWrite(0x00, 0xB0);
    memWrite(RAM_SIZE-1, 0x0B);
    EXPECT_EQ(cpu.memRead(0x00), 0xB0);
    EXPECT_EQ(cpu.memRead(RAM_SIZE-1), 0x0B);
}

TEST_F(CPUTest, FetchIncrementsPCBy2Bytes){
    EXPECT_EQ(cpu.PCRead(), 0x0200);
    EXPECT_EQ(fetch(), 0x0000);
    EXPECT_EQ(cpu.PCRead(), 0x0202);
}

TEST_F(CPUTest, FetchReturnsTheNextInstruction){
    memWrite(0x200, 0xB0);
    memWrite(0x201, 0x0B);
    EXPECT_EQ(fetch(), 0xB00B);
}