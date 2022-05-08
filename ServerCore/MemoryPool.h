#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	MemoryHeader(int32 size) : alloc_size(size) {}

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);

		// ������ ���� Ư���� �޸� ����� �ǳʶٰ� �����͸� ����Ű�� �ּҸ� �Ѱ���
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 alloc_size;
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 alloc_size);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader* Pop();

private:
	SLIST_HEADER	header_;
	int32			alloc_size_ = 0;
	atomic<int32>	alloc_count_ = 0;
	atomic<int32>	reserved_count_ = 0;
};
