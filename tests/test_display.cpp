#include <gtest/gtest.h>
#include <array>
#include "CHIP_8/display.hpp"

using namespace std;

class DisplayTest : public testing::Test{
    protected:
        Display display;
        vector<pixel_t>& buffer;

    public: 
        DisplayTest() : display(), buffer(display.buffer){}
};

TEST_F(DisplayTest, ScreenIsFullyBlackWhenInitialized){
    int i=0;
    for (i=0; i<SCREEN_SIZE; i++){
        ASSERT_EQ(buffer.at(i), 0x00);
    }
}

TEST_F(DisplayTest, CanTogglePixelsOnAndOffByPositionOnScreen){
    ASSERT_EQ(buffer.at(0), 0x00);
    display.togglePixel(0, 0);
    ASSERT_EQ(buffer.at(0), 0xFF);

    ASSERT_EQ(buffer.at(SCREEN_SIZE-1), 0x00);
    display.togglePixel(SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
    ASSERT_EQ(buffer.at(SCREEN_SIZE-1), 0xFF);
}

TEST_F(DisplayTest, CanTogglePixelsOnAndOffByPixelIndex){
    ASSERT_EQ(buffer.at(0), 0x00);
    display.togglePixel(0);
    ASSERT_EQ(buffer.at(0), 0xFF);

    ASSERT_EQ(buffer.at(SCREEN_SIZE-1), 0x00);
    display.togglePixel(SCREEN_SIZE-1);
    ASSERT_EQ(buffer.at(SCREEN_SIZE-1), 0xFF);
}

TEST_F(DisplayTest, ConGetAPixelsValueWithGetPixel){
    ASSERT_EQ(display.getPixel(0, 0), 0x00);
    display.togglePixel(0, 0);
    ASSERT_EQ(display.getPixel(0, 0), 0xFF);
}