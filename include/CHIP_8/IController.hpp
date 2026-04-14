#pragma once

#include "commons.hpp"

enum KEYS{
      KEY_1 = 1, KEY_2 = 2, KEY_3 = 3, KEY_C = 0xC, 
      KEY_4 = 4, KEY_5 = 5, KEY_6 = 6, KEY_D = 0xD, 
      KEY_7 = 7, KEY_8 = 8, KEY_9 = 9, KEY_E = 0xE, 
    KEY_A = 0xA, KEY_0 = 0, KEY_B = 0xB, KEY_F = 0xF,

    NO_KEY = 0xFF
};

class IController{
    public:
        virtual ~IController() = default;

        // Returns true if the given key is pressed. Returns false otherwise
        virtual bool isPressed(byte_t key) const = 0;

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
        virtual KEYS getPressedKey() const = 0;
};