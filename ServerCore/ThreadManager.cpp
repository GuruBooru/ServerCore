#include "pch.h"
#include "ThreadManager.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

ThreadManager::ThreadManager()
{
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(lock_);

	threads_.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (thread& t : threads_)
	{
		if (t.joinable())
			t.join();
	}
	threads_.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> s_thread_id = 1;
	l_thread_id = s_thread_id.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}

void ThreadManager::DoGlobalQueue()
{
	while (true)
	{
		uint64 now = ::GetTickCount64();

		if (now > l_global_queue_end)
			break;

		JobQueueRef job_queue = g_global_queue->Pop();
		if (job_queue == nullptr)
			break;

		job_queue->Execute();
	}
}
