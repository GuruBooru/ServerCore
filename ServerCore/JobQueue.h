#pragma once
#include "Job.h"
#include "LockQueue.h"

class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(CallbackType&& callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::*mem_func)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, mem_func, std::forward<Args>(args)...));
	}

	void ClearJobs() { jobs_.Clear(); }
	void Execute();
protected:
	LockQueue<JobRef> jobs_;
	Atomic<int32> job_count_ = 0;

private:
	void Push(JobRef&& job);
};

