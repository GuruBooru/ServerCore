#pragma once

class GlobalQueue
{
public:
	GlobalQueue();
	~GlobalQueue();

	void		Push(JobQueueRef job_queue);
	JobQueueRef Pop();

private:
	LockQueue<JobQueueRef> job_queues_;
};

