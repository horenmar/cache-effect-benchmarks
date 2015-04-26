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
#include "min_LCG.h"

constexpr std::size_t rep_count = 10;
constexpr std::size_t smallest_sequence = 1 << 3;
constexpr std::size_t largest_sequence = 1 << 27;
constexpr std::size_t smallest_map = 1 << 3;
constexpr std::size_t largest_map = 1 << 25; //Separate from sequence, because at 1 << 27, maps were untractable.
constexpr std::size_t smallest_matrix = 1 << 1;
constexpr std::size_t largest_matrix = 1 << 11;
constexpr std::size_t smallest_step = 1 << 0;
constexpr std::size_t largest_step = 1 << 10;

using measurements = std::vector<std::pair<int, std::uint64_t>>;

template <typename Container>
measurements measure_iteration(std::size_t start_at, std::size_t end_at);
template <typename MultiplyMethod>
measurements measure_matrix_multiplication(std::size_t start_at, std::size_t end_at, MultiplyMethod method);
template <typename Container>
measurements measure_reversed_iteration(std::size_t start_at, std::size_t end_at);
measurements measure_vector_skip(std::size_t first_step, std::size_t last_step);
measurements measure_random_iteration(std::size_t start_at, std::size_t end_at);
template <typename Container, std::size_t N_reads, std::size_t N_writes>
measurements measure_random_access(std::size_t start_at, std::size_t end_at);


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

void vector_element_skip(std::ostream& out){
    auto results = measure_vector_skip(smallest_step, largest_step);
    out << "N,\t\tVector Stepping\n";
    print_results(out, results);
}

void random_sum_vector(std::ostream& out){
    auto results = measure_vector_skip(smallest_step, largest_step);
    out << "N,\t\tRandom Iteration\n";
    print_results(out, results);
}

void read_map(std::ostream& out){
    auto results = measure_random_access<std::map<int, int>, 1, 0>(smallest_map, largest_map);
    out << "N,\t\tRead Map (1 : 0 (read only))\n";
    print_results(out, results);
}
void read_flatmap(std::ostream& out){
    auto results = measure_random_access<flatmap<int, int>, 1, 0>(smallest_map, largest_map);
    out << "N,\t\tRead Flatmap (1 : 0 (read only))\n";
    print_results(out, results);
}
void read_write_map(std::ostream& out){
    auto results = measure_random_access<std::map<int, int>, 1, 1>(smallest_map, largest_map);
    out << "N,\t\tRead Map (1 : 1 (read, write))\n";
    print_results(out, results);
}
void read_write_flatmap(std::ostream& out){
    auto results = measure_random_access<flatmap<int, int>, 1, 1>(smallest_map, largest_map);
    out << "N,\t\tRead Flatmap (1 : 1 (read, write))\n";
    print_results(out, results);
}
void read_heavy_map(std::ostream& out){
    auto results = measure_random_access<std::map<int, int>, 15, 1>(smallest_map, largest_map);
    out << "N,\t\tRead Map (15 : 1 (read heavy))\n";
    print_results(out, results);
}
void read_heavy_flatmap(std::ostream& out){
    auto results = measure_random_access<flatmap<int, int>, 15, 1>(smallest_map, largest_map);
    out << "N,\t\tRead Flatmap (15 : 1 (read heavy))\n";
    print_results(out, results);
}


using bencher = void (*)(std::ostream&);
std::map<std::string, bencher> benches = {
    {"reverse_sum_list", reverse_sum_list},
    {"reverse_sum_vector", reverse_sum_vector},
    {"smarter_matrix_multiply", smarter_matrix_multiply},
    {"naive_matrix_multiply", naive_matrix_multiply},
    {"sequential_sum_list", sequential_sum_list},
    {"sequential_sum_vector", sequential_sum_vector},
    {"vector_element_skip", vector_element_skip},
    {"random_sum_vector", random_sum_vector},
    {"read_map", read_map},
    {"read_flatmap", read_flatmap},
    {"read_write_map", read_write_map},
    {"read_write_flatmap", read_write_flatmap},
    {"read_heavy_map", read_heavy_map},
    {"read_heavy_flatmap", read_heavy_flatmap}
};



void print_help() {
    std::cerr << "Specify a benchmark:" << std::endl;
    for (const auto& test : benches) {
        std::cerr << "    " << test.first << std::endl;
    }
}

int main(int argc, char** argv){

    std::vector<std::string> args(argv+1, argv+argc);
    if (args.size() == 0) {
        print_help();
        return 1;
    }
    for (const auto& arg : args) {
        if (benches.find(arg) == end(benches)) {
            std::cerr << "I don't understand '" << arg << "'. " << std::endl;
            print_help();
            return 1;
        }
    }
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
measurements measure_iteration(std::size_t start_at, std::size_t end_at){

    //clamp the results
    start_at = lower_power_of_2(std::max(start_at, smallest_sequence));
    end_at = upper_power_of_2(std::min(end_at, largest_sequence));

    measurements results;
    results.reserve(32);

    for (auto n = end_at; n >= start_at; n /= 2){
        auto data = generate_random_sequence(n);
        Container test_data(begin(data), end(data));
        auto time = bench([=](){return std::accumulate(begin(test_data), end(test_data), 0);}, rep_count).count();
        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;
}

/* Measures reversed iteration+summation speed of list and vector.
 *
 * start_at is first converted to nearest, lower power of two and then it is clamped at 8
 * end_at is first converted to nearest, higher power of two and then it is clamped at 2**27 to prevent excessive memory pressure.
 *
 *
 * Returns range of <size, ns taken> values.
 */
template <typename Container>
measurements measure_reversed_iteration(std::size_t start_at, std::size_t end_at){

    //clamp the results
    start_at = lower_power_of_2(std::max(start_at, smallest_sequence));
    end_at = upper_power_of_2(std::min(end_at, largest_sequence));

    measurements results;
    results.reserve(32);

    for (auto n = end_at; n >= start_at; n /= 2){
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
measurements measure_matrix_multiplication(std::size_t start_at, std::size_t end_at, MultiplyMethod method){
    //clamp the results
    start_at = lower_power_of_2(std::max(start_at, smallest_matrix));
    end_at = upper_power_of_2(std::min(end_at, largest_matrix));

    measurements results;
    results.reserve(16);

    for (auto n = end_at; n >= start_at; n /= 2){
        auto matrix1 = generate_matrix(n, n);
        auto matrix2 = generate_matrix(n, n);

        auto time = bench([=](){return method(matrix1, matrix2).columns();}, rep_count).count();
        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;

}

/* First attempt at implementing skipping iteration.
 *
 * Might need to be changed to dirty cache lines before final benchmarking.
 */
measurements measure_vector_skip(std::size_t first_step, std::size_t last_step){
    first_step = lower_power_of_2(std::max(first_step, smallest_step));
    last_step = upper_power_of_2(std::min(last_step, largest_step));

    measurements results;
    results.reserve(32);

    for (auto step_size = first_step; step_size <= last_step; step_size *= 2){
        auto data = generate_random_sequence(largest_sequence);
        auto time = bench([=](){
            uint32_t result = 0;
            for (std::size_t i = 0; i < largest_sequence; i += step_size){
                result += data[i];
            }
            return result;
        }, rep_count).count();
        results.emplace_back(step_size, time);
    }
    return results;
}

/* A first attempt at implementing random iteration.
 *
 * Hopefully the LCG-based RNG isn't too complicated to skew the results too much.
 */
measurements measure_random_iteration(std::size_t start_at, std::size_t end_at){
    start_at = lower_power_of_2(std::max(start_at, smallest_sequence));
    end_at = upper_power_of_2(std::min(end_at, largest_sequence));

    LCG RNG;
    measurements results;
    results.reserve(32);

    for (auto n = end_at; n >= start_at; n /= 2){
        auto data = generate_random_sequence(n);
        auto mask = n - 1;
        auto time = bench([=, &RNG](){
            uint32_t temp = 0;
            for (std::size_t i = 0; i < n; ++i){
                temp += data[RNG.get_next() & mask];
            }
            return temp;
        }, rep_count).count();
        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;
}

/* Random reads, writes.
 */
template <typename Container, std::size_t N_reads, std::size_t N_writes>
measurements measure_random_access(std::size_t start_at, std::size_t end_at){
    static constexpr auto N_total = N_reads + N_writes;
    static_assert(is_power_of_2(N_total), "N_reads + N_writes must be a power of two.");
    start_at = lower_power_of_2(std::max(start_at, std::max(smallest_sequence, N_total)));
    end_at = upper_power_of_2(std::min(end_at, largest_sequence));

    LCG RNG;
    measurements results;
    results.reserve(32);

    for (auto n = end_at; n >= start_at; n /= 2){
        auto data_gen = generate_random_pairs(n);
        Container data(data_gen.begin(), data_gen.end());
        auto mask = n - 1;

        auto time = bench([=, &RNG, &data](){
            uint32_t temp = 0;

            for (std::size_t i = 0; i < n; i += N_total){
                for (std::size_t j = 0; j < N_reads; ++j){
                    temp += data.find(RNG.get_next() & mask)->second;
                }

                for (std::size_t j = 0; j < N_writes; ++j) {
                    data.find(RNG.get_next() & mask)->second = temp;
                }
            }

            return temp;
        }, rep_count).count();

        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;
}
