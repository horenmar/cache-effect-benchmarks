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
#include "cogs/types/counting_iterator.hpp"

#include "benchmarks.hpp"



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
void read_write_map(std::ostream& out){
    auto results = measure_random_access<std::map<int, int>, 1, 1>(smallest_map, largest_map);
    out << "N,\t\tRead Map (1 : 1 (read, write))\n";
    print_results(out, results);
}
void read_heavy_map(std::ostream& out){
    auto results = measure_random_access<std::map<int, int>, 15, 1>(smallest_map, largest_map);
    out << "N,\t\tRead Map (15 : 1 (read heavy))\n";
    print_results(out, results);
}
void read_flatmap(std::ostream& out){
    auto results = measure_random_access<flatmap<int, int>, 1, 0>(smallest_map, largest_map);
    out << "N,\t\tRead Flatmap (1 : 0 (read only))\n";
    print_results(out, results);
}
void read_write_flatmap(std::ostream& out){
    auto results = measure_random_access<flatmap<int, int>, 1, 1>(smallest_map, largest_map);
    out << "N,\t\tRead Flatmap (1 : 1 (read, write))\n";
    print_results(out, results);
}
void read_heavy_flatmap(std::ostream& out){
    auto results = measure_random_access<flatmap<int, int>, 15, 1>(smallest_map, largest_map);
    out << "N,\t\tRead Flatmap (15 : 1 (read heavy))\n";
    print_results(out, results);
}

void branch_sums_64k(std::ostream& out){
    auto results = measure_branch_sums<64 * 1024>();
    out << "Threshold,\t\tMeasure branch sums (64k array)\n";
    print_results(out, results);
}
void branch_sums_4M(std::ostream& out){
    auto results = measure_branch_sums<4 * 1024 * 1024>();
    out << "Threshold,\t\tMeasure branch sums (4M array)\n";
    print_results(out, results);
}
void branch_sums_1G(std::ostream& out){
    auto results = measure_branch_sums<1024 * 1024 * 1024>();
    out << "Threshold,\t\tMeasure branch sums (1G array)\n";
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
    {"read_write_map", read_write_map},
    {"read_heavy_map", read_heavy_map},
    {"read_flatmap", read_flatmap},
    {"read_write_flatmap", read_write_flatmap},
    {"read_heavy_flatmap", read_heavy_flatmap},
    {"branch_sums_64k", branch_sums_64k},
    {"branch_sums_4M", branch_sums_4M},
    {"branch_sums_1G", branch_sums_1G}
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

