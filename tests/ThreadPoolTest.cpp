#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <vector>

#include "ThreadPool.h"
#include "TaskQueue.h"


class ThreadPoolTest : public ::testing::Test {
protected:

    TaskQueue queue;
};


// --------------------------------------------------
// Test 1: Pool starts and shuts down correctly
// --------------------------------------------------

TEST_F(ThreadPoolTest, StartsAndStopsCorrectly) {

    ThreadPool pool(queue, 2);

    pool.start();

    queue.setFinished();

    EXPECT_NO_THROW(pool.wait());
}


// --------------------------------------------------
// Test 2: Single task is processed
// --------------------------------------------------

TEST_F(ThreadPoolTest, ProcessesSingleTask) {

    const std::string filename =
        "threadpool_test_1.txt";

    {
        std::ofstream file(filename);

        file << "Hello world\n";
        file << "This is a test\n";
    }


    ThreadPool pool(queue, 2);

    pool.start();

    queue.push(filename);
    queue.setFinished();

    pool.wait();


    std::vector<FileStats> results =
        pool.collectResults();

    std::vector<ProcessingError> errors =
        pool.collectErrors();


    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(errors.size(), 0);

    if (!results.empty()) {

        EXPECT_EQ(results[0].filename, filename);
        EXPECT_EQ(results[0].lines, 2);
        EXPECT_EQ(results[0].words, 6);
    }


    std::remove(filename.c_str());
}


// --------------------------------------------------
// Test 3: Multiple tasks are processed
// --------------------------------------------------

TEST_F(ThreadPoolTest, ProcessesMultipleTasks) {

    constexpr int numberOfFiles = 10;

    std::vector<std::string> filenames;


    for (int i = 0; i < numberOfFiles; ++i) {

        std::string filename =
            "threadpool_test_" +
            std::to_string(i) +
            ".txt";

        filenames.push_back(filename);


        std::ofstream file(filename);

        file << "This is test file " << i << "\n";
        file << "Second line of file " << i << "\n";
    }


    ThreadPool pool(queue, 4);

    pool.start();


    for (const auto& filename : filenames) {

        queue.push(filename);
    }

    queue.setFinished();

    pool.wait();


    std::vector<FileStats> results =
        pool.collectResults();

    std::vector<ProcessingError> errors =
        pool.collectErrors();


    EXPECT_EQ(results.size(), numberOfFiles);
    EXPECT_EQ(errors.size(), 0);


    for (const auto& filename : filenames) {

        bool found = false;

        for (const auto& result : results) {

            if (result.filename == filename) {

                found = true;
                break;
            }
        }

        EXPECT_TRUE(found);
    }


    for (const auto& filename : filenames) {

        std::remove(filename.c_str());
    }
}


// --------------------------------------------------
// Test 4: Invalid files are captured as errors
// --------------------------------------------------

TEST_F(ThreadPoolTest, CapturesProcessingErrors) {

    const std::string invalidFilename =
        "this_file_should_not_exist_12345.txt";


    ThreadPool pool(queue, 2);

    pool.start();

    queue.push(invalidFilename);
    queue.setFinished();

    pool.wait();


    std::vector<FileStats> results =
        pool.collectResults();

    std::vector<ProcessingError> errors =
        pool.collectErrors();


    EXPECT_EQ(results.size(), 0);
    EXPECT_EQ(errors.size(), 1);


    if (!errors.empty()) {

        EXPECT_EQ(
            errors[0].filename,
            invalidFilename
        );

        EXPECT_FALSE(
            errors[0].message.empty()
        );
    }
}


// --------------------------------------------------
// Test 5: Tasks are processed concurrently
// --------------------------------------------------

TEST_F(ThreadPoolTest, ProcessesManyTasksWithMultipleWorkers) {

    constexpr int numberOfFiles = 50;

    std::vector<std::string> filenames;


    for (int i = 0; i < numberOfFiles; ++i) {

        std::string filename =
            "threadpool_concurrent_" +
            std::to_string(i) +
            ".txt";

        filenames.push_back(filename);


        std::ofstream file(filename);

        for (int line = 0; line < 100; ++line) {

            file << "This is a test line containing "
                 << "several words for processing.\n";
        }
    }


    ThreadPool pool(queue, 4);

    pool.start();


    for (const auto& filename : filenames) {

        queue.push(filename);
    }

    queue.setFinished();

    pool.wait();


    std::vector<FileStats> results =
        pool.collectResults();

    std::vector<ProcessingError> errors =
        pool.collectErrors();


    EXPECT_EQ(results.size(), numberOfFiles);
    EXPECT_EQ(errors.size(), 0);


    std::remove(filenames[0].c_str());

    for (const auto& filename : filenames) {

        std::remove(filename.c_str());
    }
}


// --------------------------------------------------
// Test 6: Results and errors can be collected
// --------------------------------------------------

TEST_F(ThreadPoolTest, CollectsResultsAndErrors) {

    const std::string validFilename =
        "valid_threadpool_test.txt";

    const std::string invalidFilename =
        "invalid_threadpool_test.txt";


    {
        std::ofstream file(validFilename);

        file << "Hello world\n";
    }


    ThreadPool pool(queue, 2);

    pool.start();


    queue.push(validFilename);
    queue.push(invalidFilename);

    queue.setFinished();

    pool.wait();


    auto results =
        pool.collectResults();

    auto errors =
        pool.collectErrors();


    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(errors.size(), 1);


    if (!results.empty()) {

        EXPECT_EQ(
            results[0].filename,
            validFilename
        );
    }


    if (!errors.empty()) {

        EXPECT_EQ(
            errors[0].filename,
            invalidFilename
        );
    }


    std::remove(validFilename.c_str());
}