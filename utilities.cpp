#include "utilities.h"

uint32_t lower_power_of_2(uint32_t x){
    if (x == 0){
        //well, this is a good question... should be UINT_MAX strictly speaking, but that would be rather surprising.
        //0 is not a power of 2.
        //1 is larger than original value.
        //So...
        return 0;
    }

    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);

    return x - (x >> 1);
}

uint32_t upper_power_of_2(uint32_t x){
    x--;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    x++;
    return x;

}
