#include <gtest/gtest.h>
#include "CHIP_8/RAM.hpp"

class RAMTest : public testing::Test{
    protected:
        RAM ram;
};

TEST_F(RAMTest, CanWriteAndReadSingleBytesWithWriteAndReadMethods){
    ram.write(0x0000, 0xB0);
    ram.write(RAM_SIZE-1, 0x0B);

    EXPECT_EQ(ram.read(0x0000), 0xB0);
    EXPECT_EQ(ram.read(RAM_SIZE-1), 0x0B);
}

TEST_F(RAMTest, CanWriteAndReadSingleBytesWithIndexOperators){
    ram[0x0000] = 0xB0;
    ram[RAM_SIZE-1] = 0x0B;

    EXPECT_EQ(ram[0x0000], 0xB0);
    EXPECT_EQ(ram[RAM_SIZE-1], 0x0B);
}

TEST_F(RAMTest, CanWriteToMemoryInBulk){
    byte_t bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    ram.bulkWrite(0x0000, 5, bytes);   
    ASSERT_EQ(ram[0], 1);
    ASSERT_EQ(ram[1], 2);
    ASSERT_EQ(ram[2], 3);
    ASSERT_EQ(ram[3], 4);
    ASSERT_EQ(ram[4], 5);

    ram.bulkWrite(0x0000, 5, bytes);
    ASSERT_EQ(ram[0], 1);
    ASSERT_EQ(ram[1], 2);
    ASSERT_EQ(ram[2], 3);
    ASSERT_EQ(ram[3], 4);
    ASSERT_EQ(ram[4], 5);
}

TEST_F(RAMTest, TryingToWriteToAnOutOfRangeAddressThrowsAnException){
    EXPECT_THROW(ram.write(RAM_SIZE, 0xFF), std::out_of_range);
}

TEST_F(RAMTest, TryingToReadFromAnOutOfRangeAddressThrowsAnException){
    EXPECT_THROW(ram.write(RAM_SIZE, 0xFF), std::out_of_range);
}