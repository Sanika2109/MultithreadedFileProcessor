#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <functional>


class Benchmark {

public:

    static double measure(
        const std::function<void()>& function
    );
};


#endif