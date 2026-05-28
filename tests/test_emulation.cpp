#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks.hpp"
#include "emulation/emulator.hpp"

using namespace std;
using namespace testing;

class EmulatorTest: public Test{
    public:
        EmulatorTest():
            ram(make_shared<NiceMock<MockRAM>>()),
            display(make_shared<NiceMock<MockDisplay>>()),
            controller(make_shared<NiceMock<MockController>>()),
            cpuUnique(make_unique<NiceMock<MockCPU>>()),
            cpu(cpuUnique.get()),
            emulator(move(cpuUnique), ram, display, controller)
        {
        }

        sptr<NiceMock<MockRAM>> ram;
        sptr<NiceMock<MockDisplay>> display;
        sptr<NiceMock<MockController>> controller;
        uptr<NiceMock<MockCPU>> cpuUnique;
        NiceMock<MockCPU>* cpu;

    protected:
        Emulator emulator;

        bool getPausedState(){return emulator.paused;}

};

TEST_F(EmulatorTest, EmulatorStartsPaused){
    ASSERT_EQ(getPausedState(), true);
}

TEST_F(EmulatorTest, CanPauseAndUnpauseEmulation){
    ASSERT_EQ(getPausedState(), true);
    emulator.unpause();
    ASSERT_EQ(getPausedState(), false);
    emulator.pause();
    ASSERT_EQ(getPausedState(), true);
}

TEST_F(EmulatorTest, CPUStepMethodIsCalledWhenOperationTimePasses){
    emulator.unpause();
    EXPECT_CALL(*cpu, step());
    emulator.processTime(T500Hz);

    EXPECT_CALL(*cpu, step()).Times(10);
    emulator.processTime(T500Hz*10.0f);
}

TEST_F(EmulatorTest, CPUStepMethodIsNotCalledIfOperationTimeHasntPassed){
    emulator.unpause();
    EXPECT_CALL(*cpu, step()).Times(0);
    emulator.processTime(T500Hz*0.99f);
}

TEST_F(EmulatorTest, CPUStepMethodIsNotCalledWhenPaused){
    emulator.pause();
    EXPECT_CALL(*cpu, step()).Times(0);
    emulator.processTime(T500Hz);
    emulator.processTime(T500Hz*10.0f);
}

TEST_F(EmulatorTest, CPUDecTimersMethodIsCalledWhenOperationTimePasses){
    emulator.unpause();
    EXPECT_CALL(*cpu, decTimers());
    emulator.processTime(T60Hz);

    EXPECT_CALL(*cpu, decTimers()).Times(10);
    emulator.processTime(T60Hz*10.0f);
}

TEST_F(EmulatorTest, CPUDecTimersMethodIsNotCalledIfOperationTimeHasntPassed){
    emulator.unpause();
    EXPECT_CALL(*cpu, decTimers()).Times(0);
    emulator.processTime(T60Hz*0.99f);
}

TEST_F(EmulatorTest, CPUDecTimersMethodIsNotCalledWhenPaused){
    emulator.pause();
    EXPECT_CALL(*cpu, decTimers()).Times(0);
    emulator.processTime(T60Hz);
    emulator.processTime(T60Hz*10.0f);
}

TEST_F(EmulatorTest, CPUDoesntCountTimeWhilePaused){
    emulator.pause();
    EXPECT_CALL(*cpu, step()).Times(0);
    EXPECT_CALL(*cpu, decTimers()).Times(0);
    emulator.processTime(T500Hz*10.0f);
    emulator.unpause();
}

// TODO: Figure out how to mock a file
// TEST_F(EmulatorTest, CanLoadAFileToRAM){
//     EXPECT_CALL(ram, bulkWrite);
//     filesystem::path filePath(".");
//     emulator.load(filePath);
// }