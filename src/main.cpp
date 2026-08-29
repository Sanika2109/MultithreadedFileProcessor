#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <cmath>

#include "FileProcessor.h"
#include "TaskQueue.h"
#include "ThreadPool.h"
#include "SingleThreadProcessor.h"
#include "Benchmark.h"
#include "CSVWriter.h"


namespace fs = std::filesystem;


// --------------------------------------------------
// Configuration
// --------------------------------------------------

constexpr int BENCHMARK_RUNS = 5;

const std::vector<std::size_t> THREAD_COUNTS = {
    1, 2, 4, 8
};


// --------------------------------------------------
// Discover files
// --------------------------------------------------

std::vector<std::string> discoverFiles(
    const std::string& directory
) {

    std::vector<std::string> files;

    for (const auto& entry :
         fs::directory_iterator(directory)) {

        if (entry.is_regular_file()) {

            files.push_back(
                entry.path().string()
            );
        }
    }

    // Deterministic ordering.
    std::sort(
        files.begin(),
        files.end()
    );

    return files;
}


// --------------------------------------------------
// Benchmark statistics
// --------------------------------------------------

struct BenchmarkStats {

    double averageTime = 0.0;

    double minimumTime = 0.0;

    double maximumTime = 0.0;
};


// --------------------------------------------------
// Run a function multiple times
// --------------------------------------------------

template <typename Function>
BenchmarkStats runRepeatedBenchmark(
    Function function,
    int numberOfRuns
) {

    std::vector<double> times;

    times.reserve(numberOfRuns);


    for (int i = 0; i < numberOfRuns; ++i) {

        double elapsed =
            Benchmark::measure(function);

        times.push_back(elapsed);
    }


    double total = 0.0;

    double minimum = times[0];

    double maximum = times[0];


    for (double time : times) {

        total += time;

        if (time < minimum) {
            minimum = time;
        }

        if (time > maximum) {
            maximum = time;
        }
    }


    BenchmarkStats stats;

    stats.averageTime =
        total / numberOfRuns;

    stats.minimumTime =
        minimum;

    stats.maximumTime =
        maximum;


    return stats;
}


// --------------------------------------------------
// Multithreaded processing output
// --------------------------------------------------

struct ProcessingOutput {

    std::vector<FileStats> results;

    std::vector<ProcessingError> errors;

    double elapsedTime = 0.0;
};


// --------------------------------------------------
// Run multithreaded processing
// --------------------------------------------------

ProcessingOutput runMultithreaded(
    const std::vector<std::string>& files,
    std::size_t numberOfThreads
) {

    TaskQueue queue;

    ThreadPool pool(
        queue,
        numberOfThreads
    );


    pool.start();


    double elapsedTime =
        Benchmark::measure(
            [&]() {

                // Submit all files.
                for (const auto& file : files) {

                    queue.push(file);
                }


                // Tell workers that no more
                // tasks will be submitted.
                queue.setFinished();


                // Wait for all workers.
                pool.wait();
            }
        );


    ProcessingOutput output;

    output.elapsedTime =
        elapsedTime;

    output.results =
        pool.collectResults();

    output.errors =
        pool.collectErrors();


    return output;
}


// --------------------------------------------------
// Thread benchmark result
// --------------------------------------------------

struct ThreadBenchmarkResult {

    std::size_t threads;

    BenchmarkStats stats;

    double speedup;
};


// --------------------------------------------------
// Thread benchmark
// --------------------------------------------------

std::vector<ThreadBenchmarkResult> runThreadBenchmark(
    const std::vector<std::string>& files,
    double singleThreadAverage
) {

    std::vector<ThreadBenchmarkResult> benchmarkResults;


    std::cout
        << "\n===== THREAD BENCHMARK =====\n";


    std::cout
        << "Runs per configuration: "
        << BENCHMARK_RUNS
        << "\n\n";


    std::cout
        << std::left
        << std::setw(10)
        << "Threads"
        << std::setw(15)
        << "Avg (s)"
        << std::setw(15)
        << "Min (s)"
        << std::setw(15)
        << "Max (s)"
        << std::setw(15)
        << "Speedup"
        << "\n";


    std::cout
        << "------------------------------------------------------------\n";


    for (std::size_t threadCount :
         THREAD_COUNTS) {


        BenchmarkStats stats =
            runRepeatedBenchmark(
                [&]() {

                    ProcessingOutput output =
                        runMultithreaded(
                            files,
                            threadCount
                        );

                    // Results are deliberately not
                    // collected here because this
                    // benchmark measures processing time.
                },
                BENCHMARK_RUNS
            );


        double speedup =
            singleThreadAverage /
            stats.averageTime;


        ThreadBenchmarkResult result;

        result.threads =
            threadCount;

        result.stats =
            stats;

        result.speedup =
            speedup;


        benchmarkResults.push_back(result);


        std::cout
            << std::fixed
            << std::setprecision(6)
            << std::left
            << std::setw(10)
            << threadCount
            << std::setw(15)
            << stats.averageTime
            << std::setw(15)
            << stats.minimumTime
            << std::setw(15)
            << stats.maximumTime
            << std::setw(15)
            << speedup
            << "x\n";
    }


    return benchmarkResults;
}


// --------------------------------------------------
// Correctness check
// --------------------------------------------------

bool checkCorrectness(
    const std::vector<FileStats>& expected,
    const std::vector<FileStats>& actual
) {

    if (expected.size() != actual.size()) {

        return false;
    }


    std::unordered_map<
        std::string,
        FileStats
    > expectedMap;


    for (const auto& result : expected) {

        expectedMap[
            result.filename
        ] = result;
    }


    for (const auto& result : actual) {

        auto it =
            expectedMap.find(
                result.filename
            );


        if (it == expectedMap.end()) {

            return false;
        }


        const FileStats& expectedResult =
            it->second;


        if (
            expectedResult.lines
            != result.lines
        ) {

            return false;
        }


        if (
            expectedResult.words
            != result.words
        ) {

            return false;
        }


        if (
            expectedResult.characters
            != result.characters
        ) {

            return false;
        }
    }


    return true;
}


// --------------------------------------------------
// Save benchmark results to CSV
// --------------------------------------------------

bool writeBenchmarkResults(
    const std::string& filename,
    const BenchmarkStats& singleThreadStats,
    const std::vector<ThreadBenchmarkResult>& results
) {

    std::ofstream file(filename);


    if (!file.is_open()) {

        return false;
    }


    file << std::fixed
         << std::setprecision(6);


    // CSV header.
    file
        << "mode,"
        << "threads,"
        << "runs,"
        << "average_seconds,"
        << "min_seconds,"
        << "max_seconds,"
        << "speedup\n";


    // Single-threaded benchmark.
    file
        << "single-threaded,"
        << "1,"
        << BENCHMARK_RUNS
        << ","
        << singleThreadStats.averageTime
        << ","
        << singleThreadStats.minimumTime
        << ","
        << singleThreadStats.maximumTime
        << ","
        << "1.000000\n";


    // Multithreaded benchmarks.
    for (const auto& result : results) {

        file
            << "multithreaded,"
            << result.threads
            << ","
            << BENCHMARK_RUNS
            << ","
            << result.stats.averageTime
            << ","
            << result.stats.minimumTime
            << ","
            << result.stats.maximumTime
            << ","
            << result.speedup
            << "\n";
    }


    return true;
}


// --------------------------------------------------
// Main
// --------------------------------------------------

int main() {

    const std::string inputDirectory =
        "benchmark_input";


    const std::string outputDirectory =
        "output";


    const std::string benchmarkDirectory =
        "benchmark_results";


    const std::string resultsFile =
        outputDirectory +
        "/results.csv";


    const std::string errorsFile =
        outputDirectory +
        "/errors.log";


    const std::string benchmarkFile =
        benchmarkDirectory +
        "/benchmark_results.csv";


    // ------------------------------------------------
    // Create directories
    // ------------------------------------------------

    fs::create_directories(
        outputDirectory
    );


    fs::create_directories(
        benchmarkDirectory
    );


    // ------------------------------------------------
    // Discover files
    // ------------------------------------------------

    std::vector<std::string> files;


    try {

        files =
            discoverFiles(
                inputDirectory
            );
    }

    catch (
        const std::exception& error
    ) {

        std::cerr
            << "Error discovering files: "
            << error.what()
            << '\n';

        return 1;
    }


    if (files.empty()) {

        std::cerr
            << "No input files found.\n";

        return 1;
    }


    std::cout
        << "Files found: "
        << files.size()
        << "\n";


    // ------------------------------------------------
    // Single-threaded benchmark
    // ------------------------------------------------

    std::cout
        << "\n===== SINGLE-THREADED BENCHMARK =====\n";


    std::cout
        << "Runs: "
        << BENCHMARK_RUNS
        << "\n";


    BenchmarkStats singleThreadStats =
        runRepeatedBenchmark(
            [&]() {

                SingleThreadProcessor::
                    processFiles(files);
            },
            BENCHMARK_RUNS
        );


    std::cout
        << std::fixed
        << std::setprecision(6);


    std::cout
        << "Average time: "
        << singleThreadStats.averageTime
        << " seconds\n";


    std::cout
        << "Minimum time: "
        << singleThreadStats.minimumTime
        << " seconds\n";


    std::cout
        << "Maximum time: "
        << singleThreadStats.maximumTime
        << " seconds\n";


    // ------------------------------------------------
    // Thread benchmark
    // ------------------------------------------------

    std::vector<ThreadBenchmarkResult>
        threadBenchmarkResults =
            runThreadBenchmark(
                files,
                singleThreadStats.averageTime
            );


    // ------------------------------------------------
    // Correctness check
    // ------------------------------------------------

    std::vector<FileStats> expectedResults;


    try {

        expectedResults =
            SingleThreadProcessor::
                processFiles(files);
    }

    catch (
        const std::exception& error
    ) {

        std::cerr
            << "Correctness baseline failed: "
            << error.what()
            << '\n';

        return 1;
    }


    ProcessingOutput correctnessOutput =
        runMultithreaded(
            files,
            4
        );


    bool correctnessPassed =
        checkCorrectness(
            expectedResults,
            correctnessOutput.results
        );


    std::cout
        << "\nCorrectness check: "
        << (
            correctnessPassed
            ? "PASSED"
            : "FAILED"
        )
        << "\n";


    if (!correctnessPassed) {

        std::cerr
            << "ERROR: Multithreaded results "
               "do not match single-threaded results.\n";

        return 1;
    }


    // ------------------------------------------------
    // Final processing
    // ------------------------------------------------

    const std::size_t numberOfThreads = 4;


    ProcessingOutput output =
        runMultithreaded(
            files,
            numberOfThreads
        );


    std::cout
        << "\n===== FINAL PROCESSING =====\n";


    std::cout
        << "Threads used: "
        << numberOfThreads
        << '\n';


    std::cout
        << "Processing time: "
        << output.elapsedTime
        << " seconds\n";


    std::cout
        << "Successful files: "
        << output.results.size()
        << '\n';


    std::cout
        << "Failed files: "
        << output.errors.size()
        << '\n';


    // ------------------------------------------------
    // Final correctness validation
    // ------------------------------------------------

    if (
        output.results.size()
        != files.size()
    ) {

        std::cerr
            << "Warning: Not all files were processed successfully.\n";
    }


    // ------------------------------------------------
    // Save application results
    // ------------------------------------------------

    if (
        !CSVWriter::writeResults(
            resultsFile,
            output.results
        )
    ) {

        std::cerr
            << "Failed to write results.\n";

        return 1;
    }


    if (
        !CSVWriter::writeErrors(
            errorsFile,
            output.errors
        )
    ) {

        std::cerr
            << "Failed to write errors.\n";

        return 1;
    }


    // ------------------------------------------------
    // Save benchmark results
    // ------------------------------------------------

    if (
        !writeBenchmarkResults(
            benchmarkFile,
            singleThreadStats,
            threadBenchmarkResults
        )
    ) {

        std::cerr
            << "Failed to write benchmark results.\n";

        return 1;
    }


    // ------------------------------------------------
    // Final speedup
    // ------------------------------------------------

    if (output.elapsedTime > 0.0) {

        const double finalSpeedup =
            singleThreadStats.averageTime /
            output.elapsedTime;


        std::cout
            << "Final speedup: "
            << finalSpeedup
            << "x\n";
    }


    // ------------------------------------------------
    // Output locations
    // ------------------------------------------------

    std::cout
        << "\nResults saved to: "
        << resultsFile
        << '\n';


    std::cout
        << "Errors saved to: "
        << errorsFile
        << '\n';


    std::cout
        << "Benchmark results saved to: "
        << benchmarkFile
        << '\n';


    std::cout
        << "\nProcessing completed successfully.\n";


    return 0;
}