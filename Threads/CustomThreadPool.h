#pragma once
#include <thread>
#include <queue>
#include <future>
#include <condition_variable>
#include <unordered_set>

using namespace std;

class CustomThreadPool
{
public:
	CustomThreadPool();
	CustomThreadPool(int numThreads);
	virtual ~CustomThreadPool();

	
	void Wait(int taskId);
	void WaitAll();

	bool IsTaskFinished(int taskId);

	template <typename Func, typename ...Args>
	int addTask(const Func& task_func, Args&&... args) {
		
		int taskIdx = lastTaskid++;

		lock_guard<mutex> q_lock(queueMutex);
		Queue.emplace(async(launch::deferred, task_func, args...), taskIdx);

	
		queueCV.notify_one();
		return taskIdx;
	}

private:

	void Run();

	vector<thread> Threads;
	queue<pair<future<void>, int>> Queue;
	int threadsNum;

	mutex queueMutex;
	condition_variable queueCV;

	unordered_set<int> finishedTasks;

	condition_variable finishedTasksId_CV;
	mutex finishedTasksIdMutex;

	atomic<bool> exit{ false };
	atomic<int64_t> lastTaskid = 0;
};

