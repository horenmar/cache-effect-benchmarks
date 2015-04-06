#pragma once
#ifndef WTF_CACHE_UTILITIES
#define WTF_CACHE_UTILITIES

#include <cstdint>

constexpr bool is_power_of_2(uint32_t x){
    return (x & (x - 1)) == 0;
}

uint32_t lower_power_of_2(uint32_t x);
uint32_t upper_power_of_2(uint32_t x);

#endif
