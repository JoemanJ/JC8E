#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks.hpp"
#include "emulation/renderer.hpp"

using namespace std;
using namespace testing;

class RendererTest: public Test{
    public:
        RendererTest():
        renderer(64, 32, 1.0f)
        {
            for(uint16_t i=0; i<64*32; i++){
                pixels.push_back(255);
            }
        }

    protected:
        Renderer renderer;
        vector<sf::Uint8>& getBuffer(){return renderer.buffer;}
        void convertPixelsFromChip8(vector<byte_t> source){
            renderer.convertPixelsFromChip8(source);
        }

        vector<byte_t> pixels;
};

TEST_F(RendererTest, RendererIsCreatedWithAllPixelsOn){
    auto buffer = getBuffer();
    for(uint16_t i = 0; i<64*32*4; i++){
        EXPECT_EQ(buffer.at(i), 255);
    }
}

TEST_F(RendererTest, PixelConversionFromChip8WorksAsExpected){
    auto& buffer = getBuffer();
    
    pixels.at(0) = 0;
    pixels.at(2) = 0;
    convertPixelsFromChip8(pixels);
 
    EXPECT_EQ(buffer.at(0), 0);
    EXPECT_EQ(buffer.at(1), 0);
    EXPECT_EQ(buffer.at(2), 0);
    EXPECT_EQ(buffer.at(3), 255);
    EXPECT_EQ(buffer.at(4), 255);
    EXPECT_EQ(buffer.at(5), 255);
    EXPECT_EQ(buffer.at(6), 255);
    EXPECT_EQ(buffer.at(7), 255);
    EXPECT_EQ(buffer.at(8), 0);
    EXPECT_EQ(buffer.at(9), 0);
    EXPECT_EQ(buffer.at(10), 0);
    EXPECT_EQ(buffer.at(11), 255);
}