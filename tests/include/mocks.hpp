#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>

#include "CHIP_8/IRAM.hpp"
#include "CHIP_8/Idisplay.hpp"
#include "CHIP_8/IController.hpp"
#include "CHIP_8/ICPU.hpp"

class MockRAM : public IRAM {
    public:
        MOCK_METHOD(void, write, (addr_t address, byte_t value), (override));
        MOCK_METHOD(byte_t, read, (addr_t address), (const, override));
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
};

class MockController: public IController {
    public:
        MOCK_METHOD(bool, isPressed, (byte_t key), (override, const));
        MOCK_METHOD(KEYS, getPressedKey, (), (override, const));
};

class MockCPU: public ICPU {
    public:
        MOCK_METHOD(void, step, (), (override));
        MOCK_METHOD(void, decTimers, (), (override));
};