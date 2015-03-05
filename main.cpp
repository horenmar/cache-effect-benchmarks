#include <iostream>

#include "matrix_multiplication.h"


int main(){
    matrix foo{{1, 2}, {3, 4}};
    matrix bar(2, 6);
    matrix baz(3, 2);

#ifdef NDEBUG
    std::cout << "NDEBUG DETECTED.\n";
#endif

    matrix barbaz = multiply_naive(bar, baz);
    matrix barnabaz = multiply_smarter(bar, baz);


    return 0;

}
