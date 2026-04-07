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
        testing::NiceMock<MockRAM> ram;
        testing::NiceMock<MockDisplay> display;
        CPU cpu;
        array<byte_t, 16>& regs;

        // Auxiliary functions to access private members
        array<byte_t, 16>& getRegs(){return cpu.regs;}
        stack<addr_t>& getStack(){return cpu.stack;}
        addr_t& getPC(){return cpu.PC;}
        addr_t& getI(){return cpu.I;}
    
    public:
        CPUTest(): ram(), display(), cpu(ram, display), regs(getRegs()){}
        
        // Public functions that expose the CPU's private functions
        void stackPush(addr_t address){cpu.stackPush(address);};
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

TEST_F(CPUTest, CPUWritesStandartFontToRAMOnInitialization){
    EXPECT_CALL(ram, bulkWrite);
    CPU cpu_ = CPU(ram, display);
}

TEST_F(CPUTest, CPUCanWriteAndReadFromRAM){
    EXPECT_CALL(ram, write).Times(2);
    memWrite(0x00, 0xB0);
    memWrite(RAM_SIZE-1, 0x0B);
    EXPECT_CALL(ram, read).Times(2);
    cpu.memRead(0x00);
    cpu.memRead(RAM_SIZE-1);
}

TEST_F(CPUTest, FetchIncrementsPCBy2Bytes){
    EXPECT_EQ(cpu.PCRead(), 0x0200);
    EXPECT_EQ(fetch(), 0x0000);
    EXPECT_EQ(cpu.PCRead(), 0x0202);
}

TEST_F(CPUTest, Instruction00E0ClearScreenCallsDisplayClear){
    EXPECT_CALL(display, clear()).Times(1);
    
    decode_execute(0x00E0);
}

TEST_F(CPUTest, InvalidInstructionThrowsException){
    EXPECT_THROW(decode_execute(0x0000),invalid_argument); // 0x0NNN is not implemented
}

TEST_F(CPUTest, Instruction1NNNJumpChangesPC){
    decode_execute(0x1ABC);
    EXPECT_EQ(cpu.PCRead(), 0x0ABC);
    decode_execute(0x1000);
    EXPECT_EQ(cpu.PCRead(), 0x0000);
    decode_execute(0x1FFF);
    EXPECT_EQ(cpu.PCRead(), 0x0FFF);
}

TEST_F(CPUTest, StackOperationsWork){
    stackPush(0xABCD);
    EXPECT_EQ(stackPop(), 0xABCD);
}

TEST_F(CPUTest, Instructions0NNNSubroutineAnd00EEReturnWork){
    stack<addr_t>& stack = getStack();
    ASSERT_EQ(cpu.PCRead(), 0x0200);

    decode_execute(0x2ABC);
    ASSERT_FALSE(stack.empty());
    EXPECT_EQ(stack.top(), 0x0200);
    EXPECT_EQ(cpu.PCRead(), 0x0ABC);

    decode_execute(0x00EE);
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(cpu.PCRead(), 0x0200);
}

TEST_F(CPUTest, Instruction3XNNSkipIfVXEqualNNWorks){
    decode_execute(0x3000);
    EXPECT_EQ(cpu.PCRead(), 0x202);

    regs.at(0xA) = 0xBC;
    decode_execute(0x3ABC);
    EXPECT_EQ(cpu.PCRead(), 0x204);

    regs.at(0xB) = 0xCD;
    decode_execute(0x3B00);
    EXPECT_EQ(cpu.PCRead(), 0x204);
}

TEST_F(CPUTest, Instruction4XNNSkipIfVXNotEqualNNWorks){
    decode_execute(0x4000);
    EXPECT_EQ(cpu.PCRead(), 0x200);

    regs.at(0xA) = 0xBC;
    decode_execute(0x4ABC);
    EXPECT_EQ(cpu.PCRead(), 0x200);

    regs.at(0xB) = 0xCD;
    decode_execute(0x4B00);
    EXPECT_EQ(cpu.PCRead(), 0x202);
}

TEST_F(CPUTest, Instruction5XY0SkipIfVXEqualVYWorks){
    decode_execute(0x5000);
    EXPECT_EQ(cpu.PCRead(), 0x202);

    regs.at(0x0) = 0x12;
    regs.at(0x1) = 0x12;
    decode_execute(0x5010);
    EXPECT_EQ(cpu.PCRead(), 0x204);

    regs.at(0x1) = 0x13;
    decode_execute(0x5010);
    EXPECT_EQ(cpu.PCRead(), 0x204);
}

TEST_F(CPUTest, Instruction9XY0SkipIfVXNotEqualVYWorks){
    decode_execute(0x9000);
    EXPECT_EQ(cpu.PCRead(), 0x200);

    regs.at(0x0) = 0x12;
    regs.at(0x1) = 0x12;
    decode_execute(0x9010);
    EXPECT_EQ(cpu.PCRead(), 0x200);

    regs.at(0x1) = 0x13;
    decode_execute(0x9010);
    EXPECT_EQ(cpu.PCRead(), 0x202);
}

TEST_F(CPUTest, Instruction6XNNSetRegisterXToNNWorks){
    decode_execute(0x60AB);
    EXPECT_EQ(regs.at(0x0), 0xAB);
    decode_execute(0x6FFF);
    EXPECT_EQ(regs.at(0xF), 0xFF);
}

TEST_F(CPUTest, Instruction7XNNAddNNToVXWorks){
    decode_execute(0x7012);
    EXPECT_EQ(regs.at(0), 0x12);
    decode_execute(0x7012);
    EXPECT_EQ(regs.at(0), 0x24);
    decode_execute(0x7012);
    EXPECT_EQ(regs.at(0), 0x36);

    regs.at(0xF) = 0xFF;
    decode_execute(0x7F01);
    EXPECT_EQ(regs.at(0xF), 0x00); // overflow
}

TEST_F(CPUTest, Instruction8XY0SetVXToVYWorks){
    regs.at(0x1) = 0x25;
    decode_execute(0x8010);
    EXPECT_EQ(regs.at(0x0), 0x25);
    
    regs.at(0xF) = 0xFF;
    decode_execute(0x81F0);
    EXPECT_EQ(regs.at(0x1), 0xFF);
}

TEST_F(CPUTest, Instruction8XY1VXBecomesBinaryORBetweenVXAndVYWorks){
    regs.at(0x1) = 0b10101010;
    decode_execute(0x8011);
    EXPECT_EQ(regs.at(0x0), 0b10101010);

    decode_execute(0x8021);
    EXPECT_EQ(regs.at(0x0), 0b10101010);

    decode_execute(0x8121);
    EXPECT_EQ(regs.at(0x1), 0b10101010);

    regs.at(0xA) = 0b11000000;
    regs.at(0xB) = 0b00110000;
    regs.at(0xC) = 0b00001100;
    regs.at(0xD) = 0b00000011;
    decode_execute(0x8AB1);
    EXPECT_EQ(regs.at(0xA), 0b11110000);
    decode_execute(0x8CD1);
    EXPECT_EQ(regs.at(0xC), 0b00001111);
    decode_execute(0x8AC1);
    EXPECT_EQ(regs.at(0xA), 0b11111111);
    EXPECT_EQ(regs.at(0xC), 0b00001111);
}

TEST_F(CPUTest, Instruction8XY2VXBecomesBinaryANDBetweenVXAndVYWorks){
    regs.at(0x1) = 0b10101010;
    regs.at(0x2) = 0b01010101;
    regs.at(0x3) = 0b00000011;
    decode_execute(0x8012);
    EXPECT_EQ(regs.at(0x0), 0b00000000);

    decode_execute(0x8122);
    EXPECT_EQ(regs.at(0x1), 0b00000000);

    regs.at(0x1) = 0b10101010;

    decode_execute(0x8132);
    EXPECT_EQ(regs.at(0x1), 0b00000010);

    decode_execute(0x8232);
    EXPECT_EQ(regs.at(0x2), 0b00000001);
}

TEST_F(CPUTest, Instruction8XY3VXBecomesBinaryXORBetweenVXAndVYWorks){
    regs.at(0x1) = 0b10101010;
    regs.at(0x2) = 0b01010101;
    regs.at(0x3) = 0b00000011;
    decode_execute(0x8013);
    EXPECT_EQ(regs.at(0x0), 0b10101010);

    decode_execute(0x8123);
    EXPECT_EQ(regs.at(0x1), 0b11111111);

    decode_execute(0x8113);
    EXPECT_EQ(regs.at(0x1), 0b00000000);
}

//TODO: turn these into integration tests

// TEST_F(CPUTest, RAMHasStandardFontFrom0x50To0x9F){
//     addr_t i = 0x50;
//     for (i=0x50; i<=0x9F; i++){
//         EXPECT_EQ(cpu.memRead(i), FONT[i-0x50]);
//     }
// }

// TEST_F(CPUTest, RAMIsAllZerosFromWhereTheStandardFontEndsOn0x9FOnward){
//     addr_t i = 0xA0;
//     for (i=0xA0; i<RAM_SIZE; i++){
//         EXPECT_EQ(cpu.memRead(i), 0x00);
//     }
// }

// TEST_F(CPUTest, FetchReturnsTheNextInstruction){
//     memWrite(0x200, 0xB0);
//     memWrite(0x201, 0x0B);
//     EXPECT_EQ(fetch(), 0xB00B);
// }