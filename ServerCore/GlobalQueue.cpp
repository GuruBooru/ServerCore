#include "pch.h"
#include "GlobalQueue.h"

GlobalQueue::GlobalQueue()
{
}

GlobalQueue::~GlobalQueue()
{
}

void GlobalQueue::Push(JobQueueRef job_queue)
{
	job_queues_.Push(job_queue);
}

JobQueueRef GlobalQueue::Pop()
{
	return job_queues_.Pop();
}
