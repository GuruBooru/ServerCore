#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 alloc_size) : alloc_size_(alloc_size)
{
	InitializeSListHead(&header_);
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(InterlockedPopEntrySList(&header_)))
	{
		_aligned_free(memory);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->alloc_size = 0;
	InterlockedPushEntrySList(&header_, static_cast<PSLIST_ENTRY>(ptr));
	alloc_count_.fetch_sub(1);
	reserved_count_.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* memory = static_cast<MemoryHeader*>(InterlockedPopEntrySList(&header_));

	if (memory == nullptr)
	{
		memory = reinterpret_cast<MemoryHeader*>(_aligned_malloc(alloc_size_, SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(memory->alloc_size == 0);
	}

	alloc_count_.fetch_add(1);
	reserved_count_.fetch_sub(1);

	return memory;
}
