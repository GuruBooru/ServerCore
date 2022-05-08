#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

/*-----------------------------------------------
				MemoryManager
-----------------------------------------------*/
MemoryManager::MemoryManager()
{
	int32 size = 0;
	int32 table_index = 0;

	for (size = SMALL_SIZE; size <= SMALL_CHUNK; size += SMALL_SIZE)
	{
		MemoryPool* pool = new MemoryPool(size);
		pools_.push_back(pool);

		while (table_index <= size)
		{
			pool_table_[table_index] = pool;
			table_index++;
		}
	}
	for (; size <= 2048; size += MEDIUM_SIZE)
	{
		MemoryPool* pool = new MemoryPool(size);
		pools_.push_back(pool);

		while (table_index <= size)
		{
			pool_table_[table_index] = pool;
			table_index++;
		}
	}
	for (; size <= MAX_ALLOC_SIZE; size += LARGE_SIZE)
	{
		MemoryPool* pool = new MemoryPool(size);
		pools_.push_back(pool);

		while (table_index <= size)
		{
			pool_table_[table_index] = pool;
			table_index++;
		}
	}
}

MemoryManager::~MemoryManager()
{
	for (MemoryPool* pool : pools_)
		delete pool;

	pools_.clear();
}

void* MemoryManager::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 alloc_size = size + sizeof(MemoryHeader);

#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(alloc_size));
#else
	// 메모리 풀링 최대 크기를 벗어난 경우
	if (alloc_size > MAX_ALLOC_SIZE)
	{
		header = reinterpret_cast<MemoryHeader*>(_aligned_malloc(alloc_size, SLIST_ALIGNMENT));
	}
	else
		header = pool_table_[alloc_size]->Pop();
#endif

	return MemoryHeader::AttachHeader(header, alloc_size);
}

void MemoryManager::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
	const int32 alloc_size = header->alloc_size;
	ASSERT_CRASH(alloc_size > 0);

#ifdef _STOMP
	StompAllocator::Release(header);
#else
	if (alloc_size > MAX_ALLOC_SIZE)
		_aligned_free(header);
	else
		pool_table_[alloc_size]->Push(header);
#endif
}
