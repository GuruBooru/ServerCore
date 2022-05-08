#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

/*-----------------------------------------------
				BaseAllocator
-----------------------------------------------*/
void* BaseAllocator::Alloc(int32 size)
{
	return malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	free(ptr);
}

/*-----------------------------------------------
				StompAllocator
-----------------------------------------------*/
void* StompAllocator::Alloc(int32 size)
{
	const int64 page_count = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 data_offset = page_count * PAGE_SIZE - size;

	void* base_address = VirtualAlloc(NULL, page_count * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return static_cast<void*>(static_cast<int8*>(base_address) + data_offset);
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 base_address = address - (address % PAGE_SIZE);
	VirtualFree(reinterpret_cast<void*>(base_address), 0, MEM_RELEASE);
}

/*-----------------------------------------------
				PoolAllocator
-----------------------------------------------*/
void* PoolAllocator::Alloc(int32 size)
{
	return g_memory_manager->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	g_memory_manager->Release(ptr);
}