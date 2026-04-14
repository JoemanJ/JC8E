#pragma once

#include "CHIP_8/IController.hpp"
#include <bitset>

class Controller: public IController{
    private:
        std::bitset<16> keys;

    public:
        void press(byte_t key) {keys.set(key);}
        void release(byte_t key) {keys.reset(key);}
        bool isPressed(byte_t key) const {return keys.test(key);}
};