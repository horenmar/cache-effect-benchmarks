#pragma once
#ifndef WTF_BENCHMARKS
#define WTF_BENCHMARKS

#include "polymorphic_bench.hpp"

constexpr std::size_t rep_count = 10;
constexpr std::size_t smallest_sequence = 1 << 3;
constexpr std::size_t largest_sequence = 1 << 25;
constexpr std::size_t smallest_map = 1 << 3;
constexpr std::size_t largest_map = 1 << 15; //Separate from sequence, because at 1 << 27, maps were untractable.
constexpr std::size_t smallest_matrix = 1 << 1;
constexpr std::size_t largest_matrix = 1 << 11;
constexpr std::size_t smallest_step = 1 << 0;
constexpr std::size_t largest_step = 1 << 10;
constexpr std::size_t smallest_poly_sequence = 1 << 8;
constexpr std::size_t largest_poly_sequence = 1 << 24;

using measurements = std::vector<std::pair<int, std::uint64_t>>;

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
        auto time = bench([&](){return std::accumulate(begin(test_data), end(test_data), 0);}, rep_count).count();
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
        auto time = bench([&](){return std::accumulate(test_data.rbegin(), test_data.rend(), 0u);}, rep_count).count();
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

        auto time = bench([&](){return method(matrix1, matrix2).columns();}, rep_count).count();
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
        auto time = bench([&](){
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
        auto time = bench([&](){
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

#include <array>

struct BFPOD {
    std::array<uint8_t, 4096> stuffing = {};
};

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
        auto read_size = n / N_total * N_reads;
        auto numbers_start = wtf::counting_iterator<int>(1, 2);
        auto numbers_end = numbers_start + read_size;
        auto write_iter = wtf::counting_iterator<int>(0, 2);
        std::vector<int> nums(numbers_start, numbers_end);
        Container data;
        std::transform(begin(nums), end(nums), std::inserter(data, data.end()), [](int i){ return std::pair<const int, BFPOD>(i, BFPOD{});});
        auto mask = read_size - 1;

        auto time = bench([=, &RNG, &data, &write_iter](){
            uint32_t temp = 0;

            for (std::size_t i = 0; i < n; i += N_total){
                for (std::size_t j = 0; j < N_reads; ++j){
                    temp += data.find(nums[(RNG.get_next() & mask)])->first;
                }

                for (std::size_t j = 0; j < N_writes; ++j) {
                    data.insert(std::make_pair(*write_iter ,BFPOD{}));
                    ++write_iter;
                }
            }

            return temp;
        }, rep_count).count();

        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;
}

template <typename Container>
measurements measure_polymorphic_container(std::size_t start_at, std::size_t end_at){
    start_at = lower_power_of_2(std::max(start_at, smallest_poly_sequence));
    end_at = upper_power_of_2(std::min(end_at, largest_poly_sequence));

    measurements results;
    results.reserve(32);

    for (auto n = end_at; n >= start_at; n /= 2){
        auto data = fill<Container>(n);
        auto time = bench([&](){
            std::uint32_t temp = 0;
            data.for_each([&](const base& el){ temp += el.foo(1);});
            return temp;
        }, rep_count).count();
        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;

}


template <typename Container>
measurements measure_write(){
    auto start_at = smallest_map;
    auto end_at = largest_map;

    measurements results;
    results.reserve(32);

    for (auto n = end_at; n >= start_at; n /= 2){
        auto numbers_start = wtf::counting_iterator<int>(1, 2);
        auto numbers_end = numbers_start + n;

        auto write_iter = wtf::counting_iterator<int>(0, 2);
        Container data;
        std::transform(numbers_start, numbers_end, std::inserter(data, data.end()), [](int i){ return std::pair<const int, BFPOD>(i, BFPOD{});});

        auto time = bench([=, &data, &write_iter](){
            uint32_t temp = 0;

            for (std::size_t i = 0; i < n; ++i){
                if (data.insert(std::make_pair(*write_iter ,BFPOD{})).second) {
                    write_iter++;
                    temp++;
                }
            }

            return temp;
        }, rep_count).count();

        results.emplace_back(n, time);
    }

    std::reverse(begin(results), end(results));
    return results;
}

#endif
