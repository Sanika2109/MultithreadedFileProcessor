#include "TaskQueue.h"

void TaskQueue::push(const std::string& task) {

    {
        std::lock_guard<std::mutex> lock(mutex);
        tasks.push(task);
    }

    condition.notify_one();
}


bool TaskQueue::pop(std::string& task) {

    std::unique_lock<std::mutex> lock(mutex);

    condition.wait(lock, [this]() {
        return !tasks.empty() || finished;
    });

    if (tasks.empty() && finished) {
        return false;
    }

    task = tasks.front();
    tasks.pop();

    return true;
}


void TaskQueue::setFinished() {

    {
        std::lock_guard<std::mutex> lock(mutex);
        finished = true;
    }

    condition.notify_all();
}