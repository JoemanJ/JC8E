#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <stack>

#include "CHIP_8/IRAM.hpp"
#include "CHIP_8/Idisplay.hpp"
#include "CHIP_8/IController.hpp"
#include "CHIP_8/ICPU.hpp"

class MockRAM : public IRAM {
    public:
        MOCK_METHOD(void, write, (addr_t address, byte_t value), (override));
        MOCK_METHOD(byte_t, read, (addr_t address), (const, override));
        MOCK_METHOD(byte_t*, getRawMemory, (), (override));
        MOCK_METHOD(void, bulkWrite, (addr_t startAddress, std::size_t size, const byte_t *data), (override));
        MOCK_METHOD(void, load, (const std::filesystem::path& path), (override));
};

class MockDisplay : public IDisplay {
    public:
        MOCK_METHOD(void, togglePixel, (byte_t x, byte_t y), (override));
        MOCK_METHOD(void, clear, (), (override));
        MOCK_METHOD(const byte_t, getWidth, (), (override, const));
        MOCK_METHOD(const byte_t, getHeight, (), (override, const));
        MOCK_METHOD(const pixel_t, getPixel, (byte_t x, byte_t y), (override, const));
        MOCK_METHOD(const std::vector<pixel_t>&, getPixels, (), (override, const));
        MOCK_METHOD(bool, getUpdatedFlag, (), (override, const));
        MOCK_METHOD(void, resetUpdatedFlag, (), (override));
        MOCK_METHOD(void, reset, (), (override));
};

class MockController: public IController {
    public:
        MOCK_METHOD(void, press, (byte_t key), (override));
        MOCK_METHOD(void, press, (KEYS key), (override));

        MOCK_METHOD(void, release, (byte_t key), (override));
        MOCK_METHOD(void, release, (KEYS key), (override));

        MOCK_METHOD(bool, isPressed, (byte_t key), (override, const));
        MOCK_METHOD(bool, isPressed, (KEYS key), (override, const));

        MOCK_METHOD(KEYS, getPressedKey, (), (override, const));
        MOCK_METHOD(void, reset, (), (override));
};

class MockCPU: public ICPU {
    public:
        MOCK_METHOD(void, step, (), (override));
        MOCK_METHOD(void, decTimers, (), (override));
        MOCK_METHOD((std::array<byte_t, 16>&), getRegs, (), (override));
        MOCK_METHOD(addr_t&, getPC, (), (override));
        MOCK_METHOD((std::stack<addr_t>&), getStack, (), (override));
        MOCK_METHOD(addr_t&, getI, (), (override));
        MOCK_METHOD(byte_t&, getDelayTimer, (), (override));
        MOCK_METHOD(byte_t&, getSoundTimer, (), (override));
        MOCK_METHOD(void, reset, (), (override));
};