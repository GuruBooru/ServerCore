#include "pch.h"
#include "CoreTLS.h"

thread_local uint32				l_thread_id = 0;
thread_local uint64				l_global_queue_end = 0;

thread_local stack<int32>		l_lock_stack;
thread_local SendBufferChunkRef	l_send_buf_chunk;
thread_local class JobQueue*	l_current_job_queue = nullptr;