#pragma once

#include "commons.hpp"

class IController{
    public:
        virtual ~IController() = default;
        virtual bool isPressed(byte_t key) const = 0;
};