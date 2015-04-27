#include <chrono>
#include <iostream>
#include <cstdlib>
#include <cstdint>

static const int arr_size = 1024;
static const int iterations = 1000;

int count_if(int limit, int* array){
    int temp = 0;
    for (int i = 0; i < arr_size; ++i){
        if (array[i] < limit){
            temp++;
        }
    }
    return temp;
}

void measure_branch_predictions(){
    std::cout << "Branch prediction benchmark.\n";
    int array[arr_size];

    for (int i = 0; i < arr_size; ++i){
        array[i] = rand() % 10;
    }

    for (int boundary = 0; boundary <= 10; ++boundary){

        auto start_time = std::chrono::steady_clock::now();
        uint32_t temp = 0;

        for (int iter = 0; iter < iterations; ++iter){
            temp += count_if(boundary, array);
        }
        auto end_time = std::chrono::steady_clock::now();

        std::cout << "Limit: " << boundary << " result: " << temp
                  << " time (ns): " << (end_time - start_time).count() << '\n';

    }
}

int main(){
    measure_branch_predictions();
}
