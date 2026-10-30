//
// Created by nikolaj on 12/29/25.
// Heavily inspired by Geeks for Geeks:  https://www.geeksforgeeks.org/cpp/thread-pool-in-cpp/
// The Code is, however, my own, and has functionalities not found on geeks for geeks,
// I have added a cancel function, a function to get the number of ongoing tasks, a function to wait for all or some threads, and a priority queue
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool {
public:
    ///Create a thread pool, and start waiting
    ///I strongly recommend using hardware_concurrency()-1 or fewer threads
    explicit ThreadPool(size_t num_threads);
    ///destructor, waits for all current tasks to finish peacefully, does NOT clear the queue
    ~ThreadPool();
    ///Add a task to the queue, priority tasks are always started first (but that does not guarantee they will finish first)
    void enqueue(std::function<void()> task,bool priority=false);
    ///Forces threadpool to quit ASAP (Does not halt ongoing threads, but clears the queues), mainly something which should be called if a task throws an exception
    void cancel();


    ///Halts the main thread until all (or all priority) tasks are done.
    ///I STRONGLY recommend only calling this from the main thread! Having multiple waiting threads has NOT been tested!
    void wait(bool all=true);
    ///How many tasks are not finished (queued + ongoing) useful for rendering loading bars
    unsigned long unfinishedTasks(bool all=true){
        std::unique_lock<std::mutex> lock(queueGuard);
        if (all)
            return standardTaskQueue.size() + workingStandardTasks.load() + priorityTaskQueue.size() + workingPriorityTasks.load();
        else
            return priorityTaskQueue.size() + workingPriorityTasks.load();
    }

private:

    ///Flag to peacefully stop when all threads are done (Does not clear the queue, does not halt tasks)
    bool stop = false;
    ///Flag to ignore waiting tasks (does not halt tasks)
    bool cancelled = false;

    ///Number of standard tasks we are actively working on
    unsigned long workingStandardTasks=0;

    ///Number of priority tasks we are actively working on
    unsigned long workingPriorityTasks=0;

    ///Guards the queues and other internal parameters
    std::mutex queueGuard;
    ///Condition variable to manage the worker threads
    std::condition_variable queueCondition;
    ///Condition variable to manage the waiting of the main thread
    std::condition_variable waitCondition;
    ///Worker threads
    std::vector<std::thread> threads;
    ///Normal priority queue
    std::queue<std::function<void()> > standardTaskQueue;
    ///High priority queue
    std::queue<std::function<void()> > priorityTaskQueue;
};


#endif //THREADPOOL_THREADPOOL_H