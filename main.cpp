#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <list>
#include <ostream>
#include <map>

#include "matrix_multiplication.h"
#include "flatmap.h"
#include "measuring_bench.h"
#include "data_generation.h"
#include "utilities.h"

constexpr std::size_t rep_count = 10;
constexpr std::size_t smallest_sequence = 8;
constexpr std::size_t largest_sequence = 10000000;
constexpr std::size_t smallest_matrix = 2;
constexpr std::size_t largest_matrix = 10000;

using measurements = std::vector<std::pair<int, std::uint64_t>>;

template <typename Container>
measurements measure_iteration(int start_at, int end_at);
template <typename MultiplyMethod>
measurements measure_matrix_multiplication(int start_at, int end_at, MultiplyMethod method);
template <typename Container>
measurements measure_reversed_iteration(int start_at, int end_at);

void print_results(std::ostream& out, const measurements& results){
    for (const auto& pair : results){
        out << pair.first << ",\t\t" << pair.second << '\n';
    }
}

void sequential_sum_vector(std::ostream& out){
    auto results = measure_iteration<std::vector<int>>(smallest_sequence, largest_sequence);
    out << "N,\t\tVector\n";
    print_results(out, results);
}

void sequential_sum_list(std::ostream& out){
    auto results = measure_iteration<std::list<int>>(smallest_sequence, largest_sequence);
    out << "N,\t\tList\n";
    print_results(out, results);
}

void naive_matrix_multiply(std::ostream& out){
    auto results = measure_matrix_multiplication(smallest_matrix, largest_matrix, multiply_naive);
    out << "N,\t\tNaive\n";
    print_results(out, results);
}

void smarter_matrix_multiply(std::ostream& out){
    auto results = measure_matrix_multiplication(smallest_matrix, largest_matrix, multiply_smarter);
    out << "N,\t\tSmarter\n";
    print_results(out, results);
}

void reverse_sum_vector(std::ostream& out){
    auto results = measure_reversed_iteration<std::vector<int>>(smallest_sequence, largest_sequence);
    out << "N,\t\tReverse Vector\n";
    print_results(out, results);
}

void reverse_sum_list(std::ostream& out) {
    auto results = measure_reversed_iteration<std::list<int>>(smallest_sequence, largest_sequence);
    out << "N,\t\tReverse List\n";
    print_results(out, results);
}

using bencher = void (*)(std::ostream&);
std::map<std::string, bencher> benches = {
    {"reverse_sum_list", reverse_sum_list},
    {"reverse_sum_vector", reverse_sum_vector},
    {"smarter_matrix_multiply", smarter_matrix_multiply},
    {"naive_matrix_multiply", naive_matrix_multiply},
    {"sequential_sum_list", sequential_sum_list},
    {"sequential_sum_vector", sequential_sum_vector}
};

int main(int argc, char** argv){

    std::vector<std::string> args(argv+1, argv+argc);
    for (const auto& arg : args){
        benches[arg](std::cout);
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
measurements measure_iteration(int start_at, int end_at){

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

template <typename Container>
measurements measure_reversed_iteration(int start_at, int end_at){

    //clamp the results
    start_at = lower_power_of_2(std::max(start_at, 8));
    end_at = upper_power_of_2(std::min(end_at, static_cast<int>(std::pow(2, 27))));

    std::vector<std::pair<int, uint64_t>> results;
    results.reserve(32);

    for (int n = end_at; n >= start_at; n /= 2){
        auto data = generate_random_sequence(n);
        Container test_data(begin(data), end(data));
        auto time = bench([=](){return std::accumulate(test_data.rbegin(), test_data.rend(), 0);}, rep_count).count();
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
measurements measure_matrix_multiplication(int start_at, int end_at, MultiplyMethod method){
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

