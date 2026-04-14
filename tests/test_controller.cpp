#include <gtest/gtest.h>
#include "CHIP_8/controller.hpp"

class ControllerTest : public testing::Test{
    protected:
        Controller controller;

    public:
        ControllerTest() : controller(){}
};

TEST_F(ControllerTest, AllKeysStartReleased){
    for(uint8_t i = 0; i<0xF; i++){
        ASSERT_EQ(controller.isPressed(i), false);
    }
}

TEST_F(ControllerTest, CanPressReleaseAndCheckKey){
    ASSERT_EQ(controller.isPressed(0x0), false);
    controller.press(0x0);
    ASSERT_EQ(controller.isPressed(0x0), true);
    controller.release(0x0);
    ASSERT_EQ(controller.isPressed(0x0), false);
}