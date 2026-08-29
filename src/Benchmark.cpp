#include "Benchmark.h"


double Benchmark::measure(
    const std::function<void()>& function
) {

    const auto start =
        std::chrono::steady_clock::now();


    function();


    const auto end =
        std::chrono::steady_clock::now();


    std::chrono::duration<double> elapsed =
        end - start;


    return elapsed.count();
}