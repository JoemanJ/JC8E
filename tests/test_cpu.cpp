#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <array>
#include "CHIP_8/CPU.hpp"
#include "CHIP_8/RAM.hpp"
#include "CHIP_8/display.hpp"
#include "commons.hpp"

using namespace std;
using namespace testing;

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
        MOCK_METHOD(const byte_t, getWidth, (), (override, const));
        MOCK_METHOD(const byte_t, getHeight, (), (override, const));
        MOCK_METHOD(const pixel_t, getPixel, (byte_t x, byte_t y), (override, const));
};

class MockController: public IController {
    public:
        MOCK_METHOD(bool, isPressed, (byte_t key), (override, const));
        MOCK_METHOD(KEYS, getPressedKey, (), (override, const));
};

class CPUTest : public Test {
    protected:
        NiceMock<MockRAM> ram;
        NiceMock<MockDisplay> display;
        NiceMock<MockController> controller;
        CPU cpu;
        array<byte_t, 16>& regs;

        // Auxiliary functions to access private members
        array<byte_t, 16>& getRegs(){return cpu.regs;}
        array<byte_t, 16>& getRegs(CPU& cpu){return cpu.regs;}
        stack<addr_t>& getStack(){return cpu.stack;}
        addr_t& getPC(){return cpu.PC;}
        addr_t& getI(){return cpu.I;}
        addr_t& getI(CPU& cpu){return cpu.I;}
        byte_t& getDelayTimer(){return cpu.delayTimer;}
        byte_t& getSoundTimer(){return cpu.soundTimer;}
    
    public:
        CPUTest(): ram(), display(), controller(), cpu(ram, display, controller), regs(getRegs()){}
        
        // Public functions that expose the CPU's private functions
        void stackPush(addr_t address){cpu.stackPush(address);};
        addr_t stackPop(){return cpu.stackPop();};
        void memWrite(addr_t address, byte_t value){cpu.memWrite(address, value);}
        instruction_t fetch(){return cpu.fetch();}
        void decode_execute(instruction_t instruction){cpu.decode_execute(instruction);}
        void decode_execute(CPU& cpu, instruction_t instruction){cpu.decode_execute(instruction);}
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
    CPU cpu_ = CPU(ram, display, controller);
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

TEST_F(CPUTest, Instructions2NNNSubroutineAnd00EEReturnWork){
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

TEST_F(CPUTest, Instruction8XY4VXBecomesVXPlusVYAndAffectsCarryFlagWorks){
    regs.at(0x1) = 1;
    regs.at(0x2) = 2;
    regs.at(0x3) = 3;
    regs.at(0x4) = 256-6;
    decode_execute(0x8014);
    EXPECT_EQ(regs.at(0x0), 1);
    decode_execute(0x8024);
    EXPECT_EQ(regs.at(0x0), 3);
    decode_execute(0x8034);
    EXPECT_EQ(regs.at(0x0), 6);

    decode_execute(0x8044);
    EXPECT_EQ(regs.at(0x0), 0);
    EXPECT_EQ(regs.at(0xF), 1);
    decode_execute(0x8034);
    EXPECT_EQ(regs.at(0x0), 3);
    EXPECT_EQ(regs.at(0xF), 0);
}

TEST_F(CPUTest, Instruction8XY7VXBecomesVYMinusVXAndAffectsCarryFlagWorks){
    regs.at(0x1) = 9;
    regs.at(0x2) = 10;
    decode_execute(0x8127);
    EXPECT_EQ(regs.at(0x1), 1);
    EXPECT_EQ(regs.at(0xF), 1);
    decode_execute(0x8127);
    EXPECT_EQ(regs.at(0x1), 9);
    EXPECT_EQ(regs.at(0xF), 1);

    decode_execute(0x8217);
    EXPECT_EQ(regs.at(0x2), 0xFF);
    EXPECT_EQ(regs.at(0xF), 0);
}

TEST_F(CPUTest, Instruction8XY6ShiftVXRightModernBehaviorWorks){
    regs.at(0x0) = 1 << 7;
    decode_execute(0x8006);
    EXPECT_EQ(regs.at(0x0), 1 << 6);
    decode_execute(0x8016);
    EXPECT_EQ(regs.at(0x0), 1 << 5);
    decode_execute(0x8026);
    EXPECT_EQ(regs.at(0x0), 1 << 4);
    decode_execute(0x8036);
    EXPECT_EQ(regs.at(0x0), 1 << 3);
    decode_execute(0x8046);
    EXPECT_EQ(regs.at(0x0), 1 << 2);
    decode_execute(0x8056);
    EXPECT_EQ(regs.at(0x0), 1 << 1);
    decode_execute(0x8066);
    EXPECT_EQ(regs.at(0x0), 1 << 0);
    decode_execute(0x8076);
    EXPECT_EQ(regs.at(0x0), 0);
    EXPECT_EQ(regs.at(0xF), 1);
    decode_execute(0x8086);
    EXPECT_EQ(regs.at(0x0), 0);
    EXPECT_EQ(regs.at(0xF), 0);
}

TEST_F(CPUTest, Instruction8XY6ShiftVXRightLegacyBehaviorWorks){
    CPU cpu(ram, display, controller, true);
    std::array<byte_t, 16>& regs = getRegs(cpu);

    regs.at(0x1) = 1 << 7;
    decode_execute(cpu, 0x8016);
    EXPECT_EQ(regs.at(0x0), 1 << 6);
    EXPECT_EQ(regs.at(0xF), 0);

    regs.at(0x1) = 1;
    decode_execute(cpu, 0x8016);
    EXPECT_EQ(regs.at(0x0), 0);
    EXPECT_EQ(regs.at(0xF), 1);
}

TEST_F(CPUTest, Instruction8XYEShiftVXLeftModernBehaviorWorks){
    regs.at(0x0) = 1;
    decode_execute(0x800E);
    EXPECT_EQ(regs.at(0x0), 1 << 1);
    decode_execute(0x801E);
    EXPECT_EQ(regs.at(0x0), 1 << 2);
    decode_execute(0x802E);
    EXPECT_EQ(regs.at(0x0), 1 << 3);
    decode_execute(0x803E);
    EXPECT_EQ(regs.at(0x0), 1 << 4);
    decode_execute(0x804E);
    EXPECT_EQ(regs.at(0x0), 1 << 5);
    decode_execute(0x805E);
    EXPECT_EQ(regs.at(0x0), 1 << 6);
    decode_execute(0x806E);
    EXPECT_EQ(regs.at(0x0), 1 << 7);
    decode_execute(0x807E);
    EXPECT_EQ(regs.at(0x0), 0);
    EXPECT_EQ(regs.at(0xF), 1);
    decode_execute(0x808E);
    EXPECT_EQ(regs.at(0x0), 0);
    EXPECT_EQ(regs.at(0xF), 0);
}

TEST_F(CPUTest, Instruction8XY6ShiftVXLeftLegacyBehaviorWorks){
    CPU cpu(ram, display, controller, true);
    std::array<byte_t, 16>& regs = getRegs(cpu);
    
    regs.at(0x1) = 1;
    decode_execute(cpu, 0x801E);
    EXPECT_EQ(regs.at(0x0), 1 << 1);
    EXPECT_EQ(regs.at(0xF), 0);

    regs.at(0x1) = 1 << 7;
    decode_execute(cpu, 0x801E);
    EXPECT_EQ(regs.at(0x0), 0);
    EXPECT_EQ(regs.at(0xF), 1);

    decode_execute(cpu, 0x800E);
    EXPECT_EQ(regs.at(0x0), 0);
    EXPECT_EQ(regs.at(0xF), 0);
}

TEST_F(CPUTest, InstructionANNNSetIndexRegisterToNNNWorks){
    decode_execute(0xA123);
    EXPECT_EQ(cpu.IRead(), 0x0123);
    decode_execute(0xAFFF);
    EXPECT_EQ(cpu.IRead(), 0x0FFF);
    decode_execute(0xABCD);
    EXPECT_EQ(cpu.IRead(), 0x0BCD);
    decode_execute(0xA000);
    EXPECT_EQ(cpu.IRead(), 0x0000);
}

TEST_F(CPUTest, InstructionBNNNJumpWithOffsetLegacyBehaviorWorks){
    CPU cpu(ram, display, controller, true);
    array<byte_t, 16>& regs = getRegs(cpu);
    
    regs.at(0x0) = 1;
    decode_execute(cpu, 0xB123);
    EXPECT_EQ(cpu.PCRead(), 0x0124);
    decode_execute(cpu, 0xBFFF);
    EXPECT_EQ(cpu.PCRead(), 0x0000);
    decode_execute(cpu, 0xBBCD);
    EXPECT_EQ(cpu.PCRead(), 0x0BCE);
    decode_execute(cpu, 0xB000);
    EXPECT_EQ(cpu.PCRead(), 0x0001);
}

TEST_F(CPUTest, InstructionBNNNJumpWithOffsetModernBehaviorWorks){
    regs.at(0x0) = 1;
    regs.at(0x1) = 2;
    regs.at(0xB) = 3;
    decode_execute(0xB123);
    EXPECT_EQ(cpu.PCRead(), 0x0125);
    decode_execute(0xBFFF);
    EXPECT_EQ(cpu.PCRead(), 0x0FFF);
    decode_execute(0xBBCD);
    EXPECT_EQ(cpu.PCRead(), 0x0BD0);
    decode_execute(0xB000);
    EXPECT_EQ(cpu.PCRead(), 0x0001);
}

TEST_F(CPUTest, InstructionDXYNDrawNPixelsTallSpritePointedToByIndexRegisterWorks){
    regs.at(0xA) = 10;
    regs.at(0xB) = 20;
    getI() = 0x500;
    ASSERT_EQ(cpu.IRead(), 0x500);

    EXPECT_CALL(display, getWidth()).WillRepeatedly(Return(64));
    EXPECT_CALL(display, getHeight()).WillRepeatedly(Return(32));

    // Toggle a single pixel
    EXPECT_CALL(ram, read(0x500)).WillOnce(Return(0b10000000));
    EXPECT_CALL(display, togglePixel(10, 20)).Times(1);
    decode_execute(0xDAB1);
    EXPECT_EQ(regs.at(0xF), 0);
    
    // Toggle several pixels
    regs.at(0xA) = 0;
    regs.at(0xB) = 10;
    getI() = 0xA00;
    EXPECT_CALL(ram, read(0xA00)).WillOnce(Return(0b11111111));
    EXPECT_CALL(display, togglePixel).Times(8);
    decode_execute(0xDAB1);
    EXPECT_EQ(regs.at(0xF), 0);

    // Collision (toggle a pixel that is already on)
    EXPECT_CALL(ram, read(0xA00)).WillOnce(Return(0b11111111));
    EXPECT_CALL(display, getPixel).Times(8).WillRepeatedly(Return(0xFF));
    EXPECT_CALL(display, togglePixel).Times(8);
    decode_execute(0xDBA1);
    EXPECT_EQ(regs.at(0xF), 1);

    // Sprites with multiple lines (N>1)
    EXPECT_CALL(ram, read(0xA00)).WillOnce(Return(0b11111111));
    EXPECT_CALL(ram, read(0xA01)).WillOnce(Return(0b11111111));
    EXPECT_CALL(display, getPixel).Times(16).WillRepeatedly(Return(0x00));
    EXPECT_CALL(display, togglePixel).Times(16);
    decode_execute(0xDAB2);
    EXPECT_EQ(regs.at(0xF), 0);

    // Wrap starting X and Y
    regs.at(0xA) = 64;
    regs.at(0xB) = 32;
    getI() = 0x123;
    EXPECT_CALL(ram, read(0x123)).WillOnce(Return(0b10000000));
    EXPECT_CALL(display, getPixel).WillOnce(Return(0xFF)).WillRepeatedly(Return(0xFF));
    EXPECT_CALL(display, togglePixel(0,0)).Times(1);
    decode_execute(0xDAB1);

    // Do not wrap sprite
    regs.at(0xA) = 63;
    regs.at(0xB) = 31;
    EXPECT_CALL(ram, read(0x123)).WillOnce(Return(0b11111111));
    EXPECT_CALL(display, getPixel).Times(1).WillOnce(Return(0x00));
    EXPECT_CALL(display, togglePixel).Times(1);
    decode_execute(0xDABF);
}

TEST_F(CPUTest, InstructionEX9ESkipInstructionIfKeyXIsPressedWorks){
    getPC() = 0x500;
    regs.at(0x0) = 0;
    regs.at(0x8) = 8;

    EXPECT_CALL(controller, isPressed(0x0)).WillOnce(Return(false));
    decode_execute(0xE09E);
    EXPECT_EQ(cpu.PCRead(), 0x500);

    EXPECT_CALL(controller, isPressed(0x8)).WillOnce(Return(true));
    decode_execute(0xE89E);
    EXPECT_EQ(cpu.PCRead(), 0x502);
}

TEST_F(CPUTest, InstructionEXA1SkipInstructionIfKeyXIsNotPressedWorks){
    getPC() = 0x500;
    regs.at(0x0) = 0;
    regs.at(0x8) = 8;

    EXPECT_CALL(controller, isPressed(0x0)).WillOnce(Return(false));
    decode_execute(0xE0A1);
    EXPECT_EQ(cpu.PCRead(), 0x502);

    EXPECT_CALL(controller, isPressed(0x8)).WillOnce(Return(true));
    decode_execute(0xE8A1);
    EXPECT_EQ(cpu.PCRead(), 0x502);
}

TEST_F(CPUTest, InstructionFX07SetVXToTheValueOfDelayTimerWorks){
    getDelayTimer() = 0xAB;
    decode_execute(0xF507);
    EXPECT_EQ(regs.at(0x5), 0xAB);
}

TEST_F(CPUTest, InstructionFX15SetDelayTimerToTheValueOfVXWorks){
    regs.at(0x5) = 0xAB;
    decode_execute(0xF515);
    EXPECT_EQ(getDelayTimer(), 0xAB);
}

TEST_F(CPUTest, InstructionFX18SetSoundTimerToTheValueOfVXWorks){
    regs.at(0x5) = 0xAB;
    decode_execute(0xF518);
    EXPECT_EQ(getSoundTimer(), 0xAB);
}

TEST_F(CPUTest, InstructionFX1EAddVXToIndexRegisterLegacyBehaviorWorks){
    CPU cpu = CPU(ram, display, controller, true);
    array<byte_t, 16>& regs = getRegs(cpu);

    getI(cpu) = 0x300;
    regs.at(0xA) = 0x25;

    decode_execute(cpu, 0xFA1E);
    EXPECT_EQ(getI(cpu), 0x325);
    EXPECT_EQ(regs.at(0xF), 0);

    getI(cpu) = 0xFFF;
    decode_execute(cpu, 0xFA1E);
    EXPECT_EQ(getI(cpu), 0x024);
    EXPECT_EQ(regs.at(0xF), 0);
}

TEST_F(CPUTest, InstructionFX1EAddVXToIndexRegisterModernBehaviorWorks){
    getI() = 0x300;
    regs.at(0xA) = 0x25;

    decode_execute(0xFA1E);
    EXPECT_EQ(getI(), 0x325);
    EXPECT_EQ(regs.at(0xF), 0);

    getI() = 0xFFF;
    decode_execute(0xFA1E);
    EXPECT_EQ(getI(), 0x024);
    EXPECT_EQ(regs.at(0xF), 1);
}

TEST_F(CPUTest, InstructionFX0AAwaitKeyPressWorks){
    regs.at(0x5) = 0xFF;
    getPC() = 0x302;

    EXPECT_CALL(controller, getPressedKey()).WillOnce(Return(KEYS::NO_KEY));
    decode_execute(0xF50A);
    EXPECT_EQ(getPC(), 0x300);
    EXPECT_EQ(regs.at(0x5), 0xFF);

    EXPECT_CALL(controller, getPressedKey()).WillOnce(Return(KEYS::KEY_0));
    decode_execute(0xF50A);
    EXPECT_EQ(getPC(), 0x300);
    EXPECT_EQ(regs.at(0x5), 0x00);

    EXPECT_CALL(controller, getPressedKey()).WillOnce(Return(KEYS::KEY_D));
    decode_execute(0xF50A);
    EXPECT_EQ(getPC(), 0x300);
    EXPECT_EQ(regs.at(0x5), 0xD);
}

TEST_F(CPUTest, InstructionFX29SetIndexRegisterToAddressOfFontCharacterInVXWorks){
    regs.at(0x0) = 0x0;
    regs.at(0x5) = 0x5;
    regs.at(0xA) = 0xA;
    regs.at(0xF) = 0xF;
    regs.at(0x7) = 0x1;

    decode_execute(0xF029);
    EXPECT_EQ(getI(), 0x50+(5*0x0));
    decode_execute(0xF529);
    EXPECT_EQ(getI(), 0x50+(5*0x5));
    decode_execute(0xFA29);
    EXPECT_EQ(getI(), 0x50+(5*0xA));
    decode_execute(0xFF29);
    EXPECT_EQ(getI(), 0x50+(5*0xF));
    decode_execute(0xF729);
    EXPECT_EQ(getI(), 0x50+(5*0x1));
}

// This is commented because it has a 1/256 chance to fail randomly
// TEST_F(CPUTest, InstructionCXNNGenerateARandomNumberANDItWithNNAndPutTheResultInVXWorks){
//     decode_execute(0xC0FF);
//     EXPECT_NE(regs.at(0x0), 0);
// }

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