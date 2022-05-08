#pragma once

#include <thread>
#include <functional>

class ThreadManager
{

public:
	ThreadManager();
	~ThreadManager();

	void			Launch(function<void(void)> callback);
	void			Join();

	static void		InitTLS();
	static void		DestroyTLS();

	static void		DoGlobalQueue();

private:
	Mutex			lock_;
	vector<thread>	threads_;
};