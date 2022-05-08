#pragma once
#include <stack>

extern thread_local uint32					l_thread_id;
extern thread_local uint64					l_global_queue_end;

extern thread_local std::stack<int32>		l_lock_stack;
extern thread_local SendBufferChunkRef		l_send_buf_chunk;
extern thread_local class JobQueue*			l_current_job_queue;