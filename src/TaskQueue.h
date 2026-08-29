#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

class TaskQueue {
private:
    std::queue<std::string> tasks;
    std::mutex mutex;
    std::condition_variable condition;
    bool finished = false;

public:
    void push(const std::string& task);

    bool pop(std::string& task);

    void setFinished();
};

#endif