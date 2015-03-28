#pragma once
#ifndef WTF_MINIMAL_LCG
#define WTF_MINIMAL_LCG

#include <cstdint>

class LCG {
public:
    uint32_t get_next(){
        make_step();
        return state;
    }

private:
    void make_step(){
        state = multiplier * state + increment;
    }

    uint32_t state = 0;
    static const uint32_t increment = 1013904223;
    static const uint32_t multiplier = 1664525;
};

#endif
