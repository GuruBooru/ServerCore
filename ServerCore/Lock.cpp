#include "pch.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#if _DEBUG
	g_deadlock_profiler->PushLock(name);
#endif

	// ������ �����尡 �����ϰ� ������ ������ ����
	const uint32 lock_thread_ID = (lock_flag_.load() & WRITE_THREAD_MASK) >> 16;
	if (l_thread_id == lock_thread_ID)
	{
		write_count_++;
		return;
	}

	// �ƹ��� ���� �� �������� ���� ��, �����Ͽ� ������ ȹ��
	const int64 begin_tick = GetTickCount64();
	const uint32 desired = ((l_thread_id << 16) & WRITE_THREAD_MASK);

	while (true)
	{
		for (uint32 spin_count = 0; spin_count < MAX_SPIN_COUNT; ++spin_count)
		{
			uint32 expected = EMPTY_FLAG;	// �ش� �Լ����� ���� ����� �� ������ ��Ÿ��
			if (lock_flag_.compare_exchange_strong(OUT expected, desired))
			{
				write_count_++;
				return;
			}
		}

		if (GetTickCount64() - begin_tick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	g_deadlock_profiler->PopLock(name);
#endif

	// ReadLock �� Ǯ�� ������ WriteUnLock �Ұ���
	if ((lock_flag_.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lock_count = --write_count_;
	if (lock_count == 0)
		lock_flag_.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	g_deadlock_profiler->PushLock(name);
#endif

	// ������ �����尡 WriteLock�� �����ϰ� �ִٸ� ����
	const uint32 lock_thread_ID = (lock_flag_.load() & WRITE_THREAD_MASK) >> 16;
	if (l_thread_id == lock_thread_ID)
	{
		lock_flag_.fetch_add(1);
		return;
	}

	// �ƹ��� ���� �����ϰ� ���� ���� ��, �����Ͽ� ���� ī��Ʈ ����
	const int64 begin_tick = GetTickCount64();
	while (true)
	{
		for (uint32 spin_count = 0; spin_count < MAX_SPIN_COUNT; ++spin_count)
		{
			uint32 expected = (lock_flag_.load() & READ_COUNT_MASK);
			if (lock_flag_.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		if (GetTickCount64() - begin_tick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	g_deadlock_profiler->PopLock(name);
#endif

	if ((lock_flag_.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
