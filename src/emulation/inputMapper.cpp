#include "emulation/inputMapper.hpp"

KeyMap defaultKeyMap{
    {kbdKey::Num1, KEY_1}, {kbdKey::Num2, KEY_2}, {kbdKey::Num3, KEY_3}, {kbdKey::Num4, KEY_C},
    {kbdKey::Q, KEY_4}, {kbdKey::W, KEY_5}, {kbdKey::E, KEY_6}, {kbdKey::R, KEY_D},
    {kbdKey::A, KEY_7}, {kbdKey::S, KEY_8}, {kbdKey::D, KEY_9}, {kbdKey::F, KEY_E},
    {kbdKey::Z, KEY_A}, {kbdKey::X, KEY_0}, {kbdKey::C, KEY_B}, {kbdKey::V, KEY_F},
};

InputMapper::InputMapper(){
    keyMap = defaultKeyMap;
}

InputMapper::InputMapper(const KeyMap& keyMap){
    this->keyMap = keyMap;
}

KEYS InputMapper::translate(const kbdKey& key) const{
    auto it = keyMap.find(key);
    if(it != keyMap.end()) return it->second;
    
    return NO_KEY;
}