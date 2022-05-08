#pragma once
#include "Types.h"

class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);

	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32>	lock_flag_ = EMPTY_FLAG;
	uint16			write_count_ = 0;
};

/*----------------
	Lock Guard
----------------*/
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) : lock_(lock), name_(name) { lock.ReadLock(name); }
	~ReadLockGuard() { lock_.ReadUnlock(name_); }
private:
	Lock& lock_;
	const char* name_;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) : lock_(lock), name_(name) { lock.WriteLock(name); }
	~WriteLockGuard() { lock_.WriteUnlock(name_); }
private:
	Lock& lock_;
	const char* name_;
};