#pragma once

class ICPU{
    public:
        virtual void step() = 0;
        virtual void decTimers() = 0;
};