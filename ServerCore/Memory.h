#pragma once
#include "Allocator.h"

class MemoryPool;

/*-----------------------------------------------
				MemoryManager
-----------------------------------------------*/
class MemoryManager
{
	enum
	{
		SMALL_SIZE = 32,
		MEDIUM_SIZE = 128,
		LARGE_SIZE = 256,

		SMALL_CHUNK = 1024,
		MEDIUM_CHUNK = 1024,
		LARGE_CHUNK = 2048,

		POOL_COUNT = (SMALL_CHUNK / SMALL_SIZE + MEDIUM_CHUNK / MEDIUM_SIZE + LARGE_CHUNK / LARGE_SIZE),
		MAX_ALLOC_SIZE = SMALL_CHUNK + MEDIUM_CHUNK + LARGE_CHUNK,
	};

public:
	MemoryManager();
	~MemoryManager();

	void* Allocate(int32 size);
	void	Release(void* ptr);

private:
	vector<MemoryPool*>		pools_;
	MemoryPool* pool_table_[MAX_ALLOC_SIZE + 1];
};

template<typename Type, typename... Args>
Type* Custom_New(Args&&... args)
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));

	// placement new
	new(memory) Type(std::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void Custom_Delete(Type* obj)
{
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args)
{
	return shared_ptr<Type> {Custom_New<Type>(forward<Args>(args)...), Custom_Delete<Type>};
}