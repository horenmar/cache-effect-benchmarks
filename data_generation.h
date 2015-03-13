#pragma once
#ifndef WTF_MATRIX_GENERATION
#define WTF_MATRIX_GENERATION

#include <vector>

#include "matrix_multiplication.h"

matrix generate_matrix(std::size_t rows, std::size_t columns, std::size_t seed = 0);
std::vector<int> generate_random_sequence(std::size_t size, std::size_t seed = 0);

#endif
