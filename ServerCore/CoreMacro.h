#pragma once

#define OUT

/*-----------------------
		Lock
-----------------------*/

#define USE_MANY_LOCKS(count)	Lock locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard read_lock_guard_##idx(locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard read_lock_guard_##idx(locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

/*-----------------------
		Memory
-----------------------*/
#ifdef _DEBUG
// 디버그 시 Stomp Alloc 사용하고자 하면 사용
//#define _STOMP
#endif

/*-----------------------
		Crash
-----------------------*/
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT CRASH");				\
		__analysis_assume(expr);			\
	}										\
}