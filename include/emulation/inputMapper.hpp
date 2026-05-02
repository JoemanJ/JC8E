#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "CHIP_8/IController.hpp"

using kbdKey = sf::Keyboard::Key;
typedef std::unordered_map<kbdKey, KEYS> KeyMap;

class InputMapper{
    public:
        /*
        Initualizes an InputMapper with the default keymap. The translation from
        keyboard keys to CHIP 8 keys in the default keymap is as follows:

        1 2 3 4          \      1 2 3 C
        Q W E R     ===== \     4 5 6 D
        A S D F     ===== /     7 8 9 E
        Z X C V          /      A 0 B F
        */
        InputMapper();

        // Initializes an InputMapper with the given keymap
        InputMapper(const KeyMap& keyMap);

        // Translates SFML keyboard key codes to CHIP 8 controller keys
        KEYS translate(const kbdKey& key) const;

    private:
        KeyMap keyMap;
};