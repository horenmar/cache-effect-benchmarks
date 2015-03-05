#pragma once
#ifndef WTF_MATRIX_MULTIPLICATION
#define WTF_MATRIX_MULTIPLICATION


#include <vector>
#include <cassert>
#include <initializer_list>

class matrix {
public:
    matrix(){}
    matrix(std::size_t rows, std::size_t columns)
    :m{rows}, n{columns}, data(m*n){}

    matrix(std::initializer_list<std::initializer_list<double>> elems)
    :m{elems.size()}, n{begin(elems)->size()} {
        for (auto& el : elems){
            data.insert(end(data), begin(el), end(el));
        }
    }

    const double& operator()(int x, int y) const {
        return data[x*y + y];
    }

    double& operator()(int x, int y){
        return data[x*y + y];
    }

    std::size_t rows() const {
        return m;
    }

    std::size_t columns() const {
        return n;
    }

private:
    std::size_t m = 0, n = 0;
    std::vector<double> data;

};

matrix multiply_naive(const matrix& lhs, const matrix& rhs);
matrix multiply_smarter(const matrix& lhs, const matrix& rhs);


#endif
