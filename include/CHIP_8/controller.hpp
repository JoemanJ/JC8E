#pragma once

#include "CHIP_8/IController.hpp"
#include <bitset>

class Controller: public IController{
    private:
        std::bitset<16> keys;

    public:
        // Changes a key's state to pressed (true)
        void press(byte_t key) {keys.set(key);}

        // Changes a key's state to released (false)
        void release(byte_t key) {keys.reset(key);}

        // Returns true if the given key is pressed. Returns false otherwise
        bool isPressed(byte_t key) const {return keys.test(key);}

        /*
        Returns NO_KEY if there are no keys pressed. Otherwise, returns the first
        key that IS pressed in the following order of priority (descending):
        0 to 9 then A to F

        Example:
        If the following keys are pressed: 9, A
        The return value will be KEY_9

        Example:
        If the following keys are pressed: E, F
        The return value will be KEY_E
        */
        KEYS getPressedKey() const;
};