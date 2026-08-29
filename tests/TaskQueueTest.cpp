#include <gtest/gtest.h>

#include <string>
#include <thread>
#include <vector>
#include <set>

#include "TaskQueue.h"


class TaskQueueTest : public ::testing::Test {
protected:

    TaskQueue queue;
};


// --------------------------------------------------
// Test 1: Push and pop a task
// --------------------------------------------------

TEST_F(TaskQueueTest, PushAndPopTask) {

    queue.push("file1.txt");

    std::string task;

    bool result = queue.pop(task);

    EXPECT_TRUE(result);
    EXPECT_EQ(task, "file1.txt");
}


// --------------------------------------------------
// Test 2: Multiple tasks
// --------------------------------------------------

TEST_F(TaskQueueTest, MultipleTasksAreProcessed) {

    queue.push("file1.txt");
    queue.push("file2.txt");
    queue.push("file3.txt");

    std::string task;

    EXPECT_TRUE(queue.pop(task));
    EXPECT_EQ(task, "file1.txt");

    EXPECT_TRUE(queue.pop(task));
    EXPECT_EQ(task, "file2.txt");

    EXPECT_TRUE(queue.pop(task));
    EXPECT_EQ(task, "file3.txt");
}


// --------------------------------------------------
// Test 3: FIFO ordering
// --------------------------------------------------

TEST_F(TaskQueueTest, MaintainsFIFOOrder) {

    queue.push("first");
    queue.push("second");
    queue.push("third");

    std::string task;

    ASSERT_TRUE(queue.pop(task));
    EXPECT_EQ(task, "first");

    ASSERT_TRUE(queue.pop(task));
    EXPECT_EQ(task, "second");

    ASSERT_TRUE(queue.pop(task));
    EXPECT_EQ(task, "third");
}


// --------------------------------------------------
// Test 4: Finished empty queue
// --------------------------------------------------

TEST_F(TaskQueueTest, FinishedQueueReturnsFalseWhenEmpty) {

    queue.setFinished();

    std::string task;

    EXPECT_FALSE(queue.pop(task));
}


// --------------------------------------------------
// Test 5: Remaining tasks are processed after finish
// --------------------------------------------------

TEST_F(TaskQueueTest, ProcessesRemainingTasksAfterFinished) {

    queue.push("file1.txt");
    queue.push("file2.txt");

    queue.setFinished();

    std::string task;

    ASSERT_TRUE(queue.pop(task));
    EXPECT_EQ(task, "file1.txt");

    ASSERT_TRUE(queue.pop(task));
    EXPECT_EQ(task, "file2.txt");

    EXPECT_FALSE(queue.pop(task));
}


// --------------------------------------------------
// Test 6: Producer-consumer concurrency
// --------------------------------------------------

TEST_F(TaskQueueTest, ProducerConsumerWorksConcurrently) {

    constexpr int numberOfTasks = 100;

    std::vector<std::string> processedTasks;

    std::thread producer([&]() {

        for (int i = 0; i < numberOfTasks; ++i) {

            queue.push(
                "file_" +
                std::to_string(i) +
                ".txt"
            );
        }

        queue.setFinished();
    });


    std::thread consumer([&]() {

        std::string task;

        while (queue.pop(task)) {

            processedTasks.push_back(task);
        }
    });


    producer.join();
    consumer.join();


    // Verify the number of tasks.
    ASSERT_EQ(
        processedTasks.size(),
        numberOfTasks
    );


    // Verify every task is unique.
    std::set<std::string> uniqueTasks(
        processedTasks.begin(),
        processedTasks.end()
    );

    EXPECT_EQ(
        uniqueTasks.size(),
        numberOfTasks
    );


    // Verify expected task names exist.
    for (int i = 0; i < numberOfTasks; ++i) {

        EXPECT_TRUE(
            uniqueTasks.count(
                "file_" +
                std::to_string(i) +
                ".txt"
            )
        );
    }
}