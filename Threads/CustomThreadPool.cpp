#include "CustomThreadPool.h"

CustomThreadPool::CustomThreadPool()
	:CustomThreadPool(2)
{
}

CustomThreadPool::CustomThreadPool(int numThreads)
	: threadsNum(numThreads)
{
    Threads.reserve(threadsNum);
    for (int i = 0; i < threadsNum; i++) 
    {
        Threads.emplace_back(&CustomThreadPool::Run, this);
    }
}

CustomThreadPool::~CustomThreadPool()
{
    exit = true;
    for (int i = 0; i < Threads.size(); i++) 
    {
        queueCV.notify_all();
        Threads[i].join();
    }
}

void CustomThreadPool::Run()
{
    while (!exit) 
    {
        unique_lock<mutex> lock(queueMutex);

    
        queueCV.wait(lock, [this]()->bool { return !Queue.empty() || exit; });

        if (!Queue.empty()) 
        {
            auto elem = move(Queue.front());
            Queue.pop();
            lock.unlock();

            elem.first.get();

            lock_guard<mutex> lock(finishedTasksIdMutex);

            finishedTasks.insert(elem.second);

            finishedTasksId_CV.notify_all();
        }
    }
}

void CustomThreadPool::Wait(int taskId)
{
    unique_lock<mutex> lock(finishedTasksIdMutex);

    finishedTasksId_CV.wait(lock, [this, taskId]()->bool
        {
        return finishedTasks.find(taskId) != finishedTasks.end();
        });
}

void CustomThreadPool::WaitAll()
{
    unique_lock<mutex> lock(queueMutex);

    finishedTasksId_CV.wait(lock, [this]()->bool {
        lock_guard<mutex> task_lock(finishedTasksIdMutex);
        return Queue.empty() && lastTaskid == finishedTasks.size();
        });
}

bool CustomThreadPool::IsTaskFinished(int taskId)
{
    lock_guard<mutex> lock(finishedTasksIdMutex);
    if (finishedTasks.find(taskId) != finishedTasks.end()) 
    {
        return true;
    }
   
    return false;
}
