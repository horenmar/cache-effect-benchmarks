#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <list>

#include "matrix_multiplication.h"
#include "flatmap.h"
#include "measuring_bench.h"
#include "data_generation.h"
#include "utilities.h"

constexpr std::size_t rep_count = 10;

template <typename Container>
std::vector<std::pair<int, std::uint64_t>> measure_iteration(int start_at, int end_at);
template <typename MultiplyMethod>
std::vector<std::pair<int, std::uint64_t>> measure_matrix_multiplication(int start_at, int end_at, MultiplyMethod method);

int main(){

    flatmap<int, int> map({{1, 2}, {2, 4}, {4, 6}});

    auto vec_results = measure_iteration<std::vector<int>>(8, 10000000);
    auto list_results = measure_iteration<std::list<int>>(8, 10000000);

    std::cout << "N\t\tVector\t\tList\n";
    for (int i = 0; i < vec_results.size(); ++i){
        std::cout << vec_results[i].first << "\t\t" << vec_results[i].second << "\t" << list_results[i].second << "\t" << static_cast<double>(vec_results[i].second) / list_results[i].second << std::endl;
    }

    std::cout << "FINISHED VECTOR X LIST.\n\n\n";

    auto naive_multiply_results = measure_matrix_multiplication(2, 10000, multiply_naive);
    auto better_multiply_results = measure_matrix_multiplication(2, 10000, multiply_smarter);

    std::cout << "N\t\tNaive\t\tBetter\n";
    for (int i = 0; i < naive_multiply_results.size(); ++i){
        std::cout << naive_multiply_results[i].first << "\t\t" << naive_multiply_results[i].second << "\t" << better_multiply_results[i].second << '\t' << static_cast<double>(naive_multiply_results[i].second) / better_multiply_results[i].second << std::endl;
    }

    return 0;

}

/* Measures iteration+summation speed of list and vector.
 *
 * start_at is first converted to nearest, lower power of two and then it is clamped at 8
 * end_at is first converted to nearest, higher power of two and then it is clamped at 2**27 to prevent excessive memory pressure.
 *
 *
 * Returns range of <size, ns taken> values.
 */
template <typename Container>
std::vector<std::pair<int, uint64_t>> measure_iteration(int start_at, int end_at){

    //clamp the results
    start_at = lower_power_of_2(std::max(start_at, 8));
    end_at = upper_power_of_2(std::min(end_at, static_cast<int>(std::pow(2, 27))));

    std::vector<std::pair<int, uint64_t>> results;
    results.reserve(32);

    for (int n = end_at; n >= start_at; n /= 2){
        auto data = generate_random_sequence(n);
        Container test_data(begin(data), end(data));
        auto time = bench([=](){return std::accumulate(begin(test_data), end(test_data), 0);}, rep_count).count();
        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;
}

/* Measures multiplication speed of matrices.
 *
 * start_at is first converted to nearest, lower power of two and then it is clamped at 2
 * end_at is first converted to nearest, higher power of two and then it is clamped at 2**11, so that the benchmarks end today.
 *
 *
 * Returns range of <size, ns taken> values.
 */
template <typename MultiplyMethod>
std::vector<std::pair<int, std::uint64_t>> measure_matrix_multiplication(int start_at, int end_at, MultiplyMethod method){
    //clam the results
    start_at = lower_power_of_2(std::max(start_at, 2));
    end_at = upper_power_of_2(std::min(end_at, static_cast<int>(std::pow(2, 11))));

    std::vector<std::pair<int, uint64_t>> results;
    results.reserve(16);

    for (int n = end_at; n >= start_at; n /= 2){
        auto matrix1 = generate_matrix(n, n);
        auto matrix2 = generate_matrix(n, n);

        auto time = bench([=](){return method(matrix1, matrix2).columns();}, rep_count).count();
        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;

}
