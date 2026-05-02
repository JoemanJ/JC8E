#include <gtest/gtest.h>
#include "emulation/inputMapper.hpp"

using namespace std;
using namespace testing;

class InputMapperTest: public Test{
    protected:
        InputMapper inputMapper;

    public:
        InputMapperTest(): inputMapper(){}
};

TEST_F(InputMapperTest, DefaultInputMapperRetrievesCorrectKeys){
    EXPECT_EQ(inputMapper.translate(kbdKey::Num1), KEYS::KEY_1);
    EXPECT_EQ(inputMapper.translate(kbdKey::Num2), KEYS::KEY_2);
    EXPECT_EQ(inputMapper.translate(kbdKey::Num3), KEYS::KEY_3);
    EXPECT_EQ(inputMapper.translate(kbdKey::Num4), KEYS::KEY_C);
    EXPECT_EQ(inputMapper.translate(kbdKey::Q), KEYS::KEY_4);
    EXPECT_EQ(inputMapper.translate(kbdKey::W), KEYS::KEY_5);
    EXPECT_EQ(inputMapper.translate(kbdKey::E), KEYS::KEY_6);
    EXPECT_EQ(inputMapper.translate(kbdKey::R), KEYS::KEY_D);
    EXPECT_EQ(inputMapper.translate(kbdKey::A), KEYS::KEY_7);
    EXPECT_EQ(inputMapper.translate(kbdKey::S), KEYS::KEY_8);
    EXPECT_EQ(inputMapper.translate(kbdKey::D), KEYS::KEY_9);
    EXPECT_EQ(inputMapper.translate(kbdKey::F), KEYS::KEY_E);
    EXPECT_EQ(inputMapper.translate(kbdKey::Z), KEYS::KEY_A);
    EXPECT_EQ(inputMapper.translate(kbdKey::X), KEYS::KEY_0);
    EXPECT_EQ(inputMapper.translate(kbdKey::C), KEYS::KEY_B);
    EXPECT_EQ(inputMapper.translate(kbdKey::V), KEYS::KEY_F);
}