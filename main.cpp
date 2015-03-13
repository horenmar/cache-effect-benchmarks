#include <iostream>

#include "matrix_multiplication.h"
#include "flatmap.h"
#include "measuring_bench.h"
#include "data_generation.h"

int main(){
    matrix foo = generate_matrix(100, 100);
    matrix bar = generate_matrix(100, 100);


    flatmap<int, int> map({{1, 2}, {2, 4}, {4, 6}});

    std::cout << "First test: " << bench([=](){return multiply_naive(foo, foo).columns();}, 1000).count() << " ns\n";
    std::cout << "Second test: " << bench([=](){return multiply_smarter(foo, foo).columns();}, 1000).count() << " ns\n";


    return 0;

}
