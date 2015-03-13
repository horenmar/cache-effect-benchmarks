#include <algorithm>
#include <iterator>
#include <random>

#include "data_generation.h"


matrix generate_matrix(std::size_t rows, std::size_t columns, std::size_t seed){
    matrix temp(rows, columns);

    std::mt19937_64 rand(seed);
    std::uniform_real_distribution<> dist(1, 100);

    for (std::size_t r = 0; r < rows; ++r){
        for (std::size_t c = 0; c < columns; ++c){
            temp(r, c) = dist(rand);
        }
    }

    return temp;
}


std::vector<int> generate_random_sequence(std::size_t size, std::size_t seed){
    std::vector<int> temp;

    std::mt19937_64 rand(seed);
    std::uniform_int_distribution<> dist(1, 100);

    std::generate_n(std::back_inserter(temp), size, [&](){return dist(rand);});

    return temp;
}


