#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"

ThreadManager*		g_thread_manager = nullptr;
MemoryManager*		g_memory_manager = nullptr;
DeadLockProfiler*	g_deadlock_profiler = nullptr;
SendBufferManager*	g_send_buf_manager = nullptr;
GlobalQueue*		g_global_queue = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		g_thread_manager = new ThreadManager();
		g_deadlock_profiler = new DeadLockProfiler();
		g_memory_manager = new MemoryManager();
		g_send_buf_manager = new SendBufferManager();
		g_global_queue = new GlobalQueue();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete g_thread_manager;
		delete g_deadlock_profiler;
		delete g_memory_manager;
		delete g_send_buf_manager;
		delete g_global_queue;
		SocketUtils::Clear();
	}
} g_core_global;