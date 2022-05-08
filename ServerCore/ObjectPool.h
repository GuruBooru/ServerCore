#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename T>
class ObjectPool
{
public:
	template<typename... Args>
	static T* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_alloc_size_));
		T* memory = static_cast<T*>(MemoryHeader::AttachHeader(ptr, s_alloc_size_));
#else
		T* memory = static_cast<T*>(MemoryHeader::AttachHeader(s_pool_.Pop(), s_alloc_size_));
#endif
		new(memory)T(forward<Args>(args)...);
		return memory;
	}

	static void Push(T* obj)
	{
		obj->~T();

#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		s_pool_.Push(MemoryHeader::DetachHeader(obj));
#endif

	}

	template<typename... Args>
	static shared_ptr<T> MakeShared(Args&&... args)
	{
		shared_ptr<T> ptr = { Pop(forward<Args>(args)...), Push };
		return ptr;
	}

private:
	static int32		s_alloc_size_;
	static MemoryPool	s_pool_;
};

template<typename T>
int32 ObjectPool<T>::s_alloc_size_ = sizeof(T) + sizeof(MemoryHeader);

template<typename T>
MemoryPool ObjectPool<T>::s_pool_{ s_alloc_size_ };