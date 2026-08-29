#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "TaskQueue.h"
#include "FileProcessor.h"

#include <thread>
#include <vector>
#include <string>
#include <cstddef>


struct ProcessingError {

    std::string filename;

    std::string message;
};


class ThreadPool {

private:

    TaskQueue& queue;

    std::vector<std::thread> workers;


    // Each worker owns its own result storage.
    std::vector<std::vector<FileStats>> workerResults;


    // Each worker owns its own errors.
    std::vector<std::vector<ProcessingError>> workerErrors;


    void workerFunction(
        std::size_t workerIndex
    );


public:

    ThreadPool(
        TaskQueue& queue,
        std::size_t numberOfThreads
    );


    void start();

    void wait();


    std::vector<FileStats> collectResults();

    std::vector<ProcessingError> collectErrors();
};


#endif