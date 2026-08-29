# Multithreaded File Processor

A C++17 application for concurrent text-file processing using a **Thread Pool** and **Producer-Consumer** architecture.

The system processes files concurrently, calculates file statistics, handles processing errors, validates correctness, and benchmarks performance across different thread configurations.

## Key Features

- C++17 multithreaded file processing
- Thread Pool architecture using `std::thread`
- Producer-Consumer task queue
- Thread synchronization using `std::mutex` and `std::condition_variable`
- Worker-local result and error storage
- Exception handling
- Single-threaded vs. multithreaded benchmarking
- 5-run performance benchmarking
- Correctness validation
- GoogleTest unit testing
- CSV-based result and benchmark reporting

## Architecture

```text
                    main.cpp
                       |
                Discover Files
                       |
             +---------+---------+
             |                   |
             v                   v
     Single-Threaded        Thread Pool
        Processor               |
             |              Task Queue
             |                   |
             |          +--------+--------+
             |          |        |        |
             |       Worker   Worker   Worker
             |          |        |        |
             |          +--------+--------+
             |                   |
             |             FileProcessor
             |                   |
             +---------+---------+
                       |
                Correctness Check
                       |
              +--------+--------+
              |                 |
          results.csv     benchmark_results.csv
````

## Project Structure

```text
MultithreadedFileProcessor/
│
├── src/
│   ├── Benchmark.*
│   ├── CSVWriter.*
│   ├── FileProcessor.*
│   ├── SingleThreadProcessor.*
│   ├── TaskQueue.*
│   ├── ThreadPool.*
│   └── main.cpp
│
├── tests/
│   ├── FileProcessorTest.cpp
│   ├── TaskQueueTest.cpp
│   └── ThreadPoolTest.cpp
│
├── input/
├── benchmark_input/
├── benchmark_results/
├── output/
├── run_tests.bat
├── .gitignore
└── README.md
```

## Performance Benchmark

The application runs each configuration **5 times** and reports average, minimum, maximum execution time, and speedup.

Tested configurations:

| Threads | Typical Speedup |
| ------: | --------------: |
|       1 |           ~1.0× |
|       2 |           ~2.2× |
|       4 |           ~3.2× |
|       8 |           ~5.4× |

A representative final run processed **50 files** successfully using 4 worker threads with approximately **3.4× speedup** over the single-threaded average.

> Performance varies depending on hardware, filesystem, workload, and system load.

Benchmark results are automatically saved to:

```text
benchmark_results/benchmark_results.csv
```

## Correctness

Multithreaded results are compared against the single-threaded baseline to verify:

* Filename
* Line count
* Word count
* Character count

Example:

```text
Correctness check: PASSED
```

## Unit Testing

GoogleTest is used for automated testing.

```text
FileProcessorTest : 5 tests
TaskQueueTest     : 6 tests
ThreadPoolTest    : 6 tests
--------------------------------
Total             : 17 tests
```

Expected:

```text
[  PASSED  ] 17 tests.
```

Run the test suite:

```bash
./run_tests.bat
```

## Build

### Requirements

* Windows
* C++17
* GCC / MinGW
* MSYS2 UCRT64
* GoogleTest

Compile:

```bash
g++ -std=c++17 \
src/main.cpp \
src/FileProcessor.cpp \
src/TaskQueue.cpp \
src/ThreadPool.cpp \
src/SingleThreadProcessor.cpp \
src/Benchmark.cpp \
src/CSVWriter.cpp \
-I./src \
-o file_processor.exe
```

Run:

```bash
./file_processor.exe
```

## Output

```text
output/
├── results.csv
└── errors.log

benchmark_results/
└── benchmark_results.csv
```