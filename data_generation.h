#pragma once
#ifndef WTF_MATRIX_GENERATION
#define WTF_MATRIX_GENERATION

#include <vector>

#include "matrix_multiplication.h"

matrix generate_matrix(std::size_t rows, std::size_t columns, std::size_t seed = 0);
std::vector<int> generate_random_sequence(std::size_t size, std::size_t seed = 0);

struct generate_random_pairs {
    const std::size_t N;
    std::vector<int> seq;

    explicit generate_random_pairs(std::size_t N, std::size_t seed = 0) :
        N(N),
        seq(generate_random_sequence(N, seed)) {}

    struct iterator {
        generate_random_pairs& par;
        int pos;
        std::pair<int, int> myp;

        explicit iterator(generate_random_pairs& par, int pos) :
                par(par),
                pos(pos),
                myp(pos, par.seq[pos]) {}

        bool operator==(const iterator& rhs) {
            return &par == &rhs.par && pos == rhs.pos;
        }

        bool operator!=(const iterator& rhs) {
            return &par != &rhs.par || pos != rhs.pos;
        }

        std::pair<int, int>& operator*() {
            return myp;
        }

        iterator& operator++() {
            ++pos;
            myp = std::make_pair(pos, par.seq[pos]);
            return *this;
        }
    };

    iterator begin() {
        return iterator(*this, 0);
    }

    iterator end() {
        return iterator(*this, N);
    }
};

namespace std {

    template <>
    struct iterator_traits<generate_random_pairs::iterator> {
        using iterator_category = std::input_iterator_tag;
    };

}

#endif
