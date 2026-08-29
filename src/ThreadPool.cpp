#include "ThreadPool.h"


ThreadPool::ThreadPool(
    TaskQueue& queue,
    std::size_t numberOfThreads
)
    : queue(queue),
      workerResults(numberOfThreads),
      workerErrors(numberOfThreads) {

    workers.reserve(numberOfThreads);
}


void ThreadPool::start() {

    for (
        std::size_t i = 0;
        i < workerResults.size();
        ++i
    ) {

        workers.emplace_back(
            &ThreadPool::workerFunction,
            this,
            i
        );
    }
}


void ThreadPool::workerFunction(
    std::size_t workerIndex
) {

    std::string filename;


    while (queue.pop(filename)) {

        try {

            FileStats result =
                processFile(filename);


            // No mutex required.
            // This vector belongs exclusively
            // to this worker.

            workerResults[workerIndex]
                .push_back(
                    std::move(result)
                );
        }


        catch (
            const std::exception& error
        ) {

            ProcessingError processingError;

            processingError.filename =
                filename;

            processingError.message =
                error.what();


            // Also worker-local.
            workerErrors[workerIndex]
                .push_back(
                    std::move(processingError)
                );
        }
    }
}


void ThreadPool::wait() {

    for (auto& worker : workers) {

        if (worker.joinable()) {

            worker.join();
        }
    }
}


std::vector<FileStats>
ThreadPool::collectResults() {

    std::vector<FileStats> allResults;


    std::size_t totalResults = 0;


    for (
        const auto& results :
        workerResults
    ) {

        totalResults += results.size();
    }


    allResults.reserve(totalResults);


    for (auto& results :
         workerResults) {

        for (auto& result :
             results) {

            allResults.push_back(
                std::move(result)
            );
        }
    }


    return allResults;
}


std::vector<ProcessingError>
ThreadPool::collectErrors() {

    std::vector<ProcessingError> allErrors;


    std::size_t totalErrors = 0;


    for (
        const auto& errors :
        workerErrors
    ) {

        totalErrors += errors.size();
    }


    allErrors.reserve(totalErrors);


    for (auto& errors :
         workerErrors) {

        for (auto& error :
             errors) {

            allErrors.push_back(
                std::move(error)
            );
        }
    }


    return allErrors;
}