#pragma once

class ICPU{
    public:
    virtual ~ICPU() = default;
        virtual void step() = 0;
        virtual void decTimers() = 0;
};