#include <chrono>

template <typename Function>
std::chrono::nanoseconds bench(Function func, int iterations){
    int temp = 0;
    //take t1;
    auto t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i){
        temp += func();
    }
    auto t2 = std::chrono::steady_clock::now();
    //write to volatile
    volatile int c = temp;
    //take t2;
    return t2-t1;
}

