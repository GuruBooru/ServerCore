#pragma once

enum
{
	S_TEST = 1,
};

class CustomPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
	static void ProcessTest(BYTE* buffer, int32 len);

private:
	// 패킷 별 동작 함수 포인터를 가지도록 수정
};

template<typename T, typename Container>
class PacketIterator
{
public:
	PacketIterator(Container& container, uint16 index) : container_(container), index_(index) {}

	bool				operator!= (const PacketIterator& other) const { return index_ != other.index_; }
	T& operator*() const { return container_[index_]; }
	T& operator*() { return container_[index_]; }
	T* operator->() { return &container_[index_]; }
	PacketIterator& operator++() { index_++; return *this; }
	PacketIterator		operator++(int32) { PacketIterator ret = *this; ++index_; return ret; }


private:
	Container& container_;
	uint16		index_;
};

template<typename T>
class PacketList
{
public:
	PacketList() : data_(nullptr), count_(0) {};
	PacketList(T* data, uint16 count) : data_(data), count_(count) {};

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < count_);
		return data_[index];
	}

	uint16								Size() { return count_; }
	PacketIterator<T, PacketList<T>>	begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>>	end() { return PacketIterator<T, PacketList<T>>(*this, count_); }
private:
	T* data_;
	uint16	count_;
};
