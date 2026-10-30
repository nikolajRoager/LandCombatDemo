//
// Created by nikolaj on 12/29/25.
//

#include <iostream>
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back([this,i]() {
            //Keep looping (until the queue is empty and we have been told to stop)
            while (true) {
                std::function<void()> task;
                //Flag tells us if we loaded the task from the priority queue (and thus should decrement priority or standard counter when done)
                bool taskIsPriority = false;
                //Limited scope kills the lock at the end, freeing the mutex for other threads
                {
                    //The lock remains on, except while waiting
                    std::unique_lock<std::mutex> lock(queueGuard);


                    //Waiting with a predicate catches spurious wake-ups
                    queueCondition.wait(lock, [this] {return (!priorityTaskQueue.empty() || !standardTaskQueue.empty() ) || stop || cancelled; });

                    //exit if the pool is to stop AND the queue is empty
                    //That means stop doesn't really mean stop now, it just means stop when you are done
                    if (stop && standardTaskQueue.empty() && priorityTaskQueue.empty())
                    {
                        return;
                    }

                    //The cancel flag on the other hand doesn't care about the remaining queue
                    if (cancelled)
                    {
                        return;
                    }
                    //Move over the next task from the queues, prioritizing priority.
                    if (priorityTaskQueue.empty()) {
                        task = std::move(standardTaskQueue.front());
                        standardTaskQueue.pop();
                        //We are actively working on one more task
                        workingStandardTasks++;
                        taskIsPriority=false;
                    }
                    else{
                        task = std::move(priorityTaskQueue.front());
                        priorityTaskQueue.pop();
                        //We are actively working on one more task
                        workingPriorityTasks++;
                        taskIsPriority=true;
                    }
                }
                //Execute the task, after freeing the mutex
                task();

                //We are done with this task, decrement the number of ongoing tasks
                {
                    std::lock_guard<std::mutex> lock(queueGuard);
                    //The overflow guards are probably not needed
                    if (taskIsPriority)
                        workingPriorityTasks= workingPriorityTasks== 0 ? 0 : workingPriorityTasks- 1;
                    else
                        workingStandardTasks = workingStandardTasks == 0 ? 0 : workingStandardTasks - 1;
                }
                //If the one or more threads are waiting, tell it to check if we are done
                waitCondition.notify_all();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    //Limited scope to unlock the mutex when we leave it
    {
        std::unique_lock<std::mutex> lock(queueGuard);
        //Flag that stops the threads from waiting, not a flag to instantly quit
        stop = true;
    }
    //It is important that the notification happens after the scope has quit
    //Otherwise the mutex would prevent the threads from leaving the wait() and we would be stuck waiting for them

    //Free all threads from wait
    queueCondition.notify_all();

    //Wait for all threads to have completed ALL tasks
    for (auto& thread : threads)
        thread.join();
}

void ThreadPool::enqueue(std::function<void()> task, bool priority) {
    //Limited scope to unlock the mutex when we leave it
    {
        std::unique_lock<std::mutex> lock(queueGuard);
        //Move the task to the queue
        if (priority)
            priorityTaskQueue.emplace(std::move(task));
        else
            standardTaskQueue.emplace(std::move(task));
    }
    //If there are any threads waiting right now, dispatch them to deal with the task
    queueCondition.notify_one();
}

void ThreadPool::cancel() {
    {
        std::unique_lock<std::mutex> lock(queueGuard);
        stop = true;
        cancelled = true;
        //Clear the queue too, since canceled means "stop soon"
        while (!standardTaskQueue.empty()) standardTaskQueue.pop();
        while (!priorityTaskQueue.empty()) priorityTaskQueue.pop();
    }
    queueCondition.notify_all();
    //When canceled also stop all waiting threads
    waitCondition.notify_all();
}

void ThreadPool::wait(bool all) {
    //The lock remains on, except while waiting
    std::unique_lock<std::mutex> lock(queueGuard);

    //Waiting with a predicate catches spurious wake-ups
    waitCondition.wait(lock, [this,all] {return unfinishedTasks(all)==0 || cancelled; });
}


