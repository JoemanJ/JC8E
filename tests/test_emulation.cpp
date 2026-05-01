#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "emulation/emulator.hpp"

using namespace std;
using namespace testing;

class EmulatorTest: public Test{
    public:
        EmulatorTest(){
        }

        CPU& cpu;
        RAM& ram;
        Display& display;
        Controller& controller;

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