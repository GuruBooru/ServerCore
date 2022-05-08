#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(JobRef&& job)
{
	const int32 prev_count = job_count_.fetch_add(1);
	jobs_.Push(job);

	if (l_current_job_queue == nullptr)
	{
		Execute();
	}
	else
	{
		g_global_queue->Push(shared_from_this());
	}
}

void JobQueue::Execute()
{
	l_current_job_queue = this;

	while (true)
	{
		Vector<JobRef> jobs;
		jobs_.PopAll(OUT jobs);

		const int32 job_count = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < job_count; ++i)
			jobs[i]->Execute();

		if (job_count_.fetch_sub(job_count) == job_count)
		{
			l_current_job_queue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= l_global_queue_end)
		{
			l_current_job_queue = nullptr;
			g_global_queue->Push(shared_from_this());
			break;
		}
	}
}
