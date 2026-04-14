#include "CHIP_8/controller.hpp"

KEYS Controller::getPressedKey() const{
    // Make a copy in case a key is released mid-process
    std::bitset<16> keys = this->keys;

    if (keys.none()) return KEYS::NO_KEY;
    if (keys.test(0)) return KEYS::KEY_0;
    if (keys.test(1)) return KEYS::KEY_1;
    if (keys.test(2)) return KEYS::KEY_2;
    if (keys.test(3)) return KEYS::KEY_3;
    if (keys.test(4)) return KEYS::KEY_4;
    if (keys.test(5)) return KEYS::KEY_5;
    if (keys.test(6)) return KEYS::KEY_6;
    if (keys.test(7)) return KEYS::KEY_7;
    if (keys.test(8)) return KEYS::KEY_8;
    if (keys.test(9)) return KEYS::KEY_9;
    if (keys.test(0xA)) return KEYS::KEY_A;
    if (keys.test(0xB)) return KEYS::KEY_B;
    if (keys.test(0xC)) return KEYS::KEY_C;
    if (keys.test(0xD)) return KEYS::KEY_D;
    if (keys.test(0xE)) return KEYS::KEY_E;
    if (keys.test(0xF)) return KEYS::KEY_F;
}