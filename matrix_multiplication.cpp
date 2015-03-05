#include "matrix_multiplication.h"


matrix multiply_naive(const matrix& lhs, const matrix& rhs){
    assert(lhs.columns() == rhs.rows() && "Dimension mismatch, cannot multiply matrices.\n");

    matrix temp(lhs.rows(), rhs.columns());
    for (int i = 0, ei = lhs.rows(); i < ei; ++i){
        for (int j = 0, ej = rhs.columns(); j < ej; ++j){
            for (int k = 0, ek = lhs.columns(); k < ek; ++k){
                temp(i, j) += lhs(i, k) * rhs(k, j);
            }
        }
    }
    return temp;
}

matrix multiply_smarter(const matrix& lhs, const matrix& rhs){
    assert(lhs.columns() == rhs.rows() && "Dimension mismatch, cannot multiply matrices.\n");

    matrix temp(lhs.rows(), rhs.columns());
    for (int i = 0, ei = lhs.rows(); i < ei; ++i){
        for (int j = 0, ej = rhs.columns(); j < ej; ++j){
            temp(i, j) = lhs(i, 0) * rhs(0, j);
        }

        for (int k = 1, ek = lhs.columns(); k < ek; ++k){
            for (int j = 0, ej = rhs.columns(); j < ej; ++j){
                temp(i, j) += lhs(i, k) * rhs(k, j);
            }
        }
    }

    return temp;
}
