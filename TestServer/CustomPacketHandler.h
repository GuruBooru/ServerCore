#pragma once
#include "BufferWriter.h"

enum
{
	S_TEST = 1,
};

class CustomPacketHandler
{
public:
	static void			HandlePacket(BYTE* buffer, int32 len);

private:
	template<typename T>
	static SendBufferRef MakeSendBuffer(T& packet, uint16 packet_id)
	{
		const uint16 data_size = static_cast<uint16>(packet.ByteSizeLong());
		const uint16 packet_size = data_size + sizeof(PacketHeader);

		SendBufferRef send_buffer = g_send_buf_manager->Open(packet_size);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(send_buffer->Buffer());
		header->size = packet_size;
		header->id = packet_id;

		ASSERT_CRASH(packet.SerializeToArray(&header[1], data_size));

		send_buffer->Close(packet_size);
		return send_buffer;
	}
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

#pragma pack(1)
struct Packet_S_TEST
{
	struct Buff
	{
		uint64 id;
		uint64 remain_time;
	};

	PacketHeader	header;
	uint64			id;
	uint32			hp;
	uint16			ap;

	uint16			buff_offset;
	uint16			buff_count;
};
#pragma pack()

class PacketTestWriter
{
public:
	using Buff = Packet_S_TEST::Buff;
	using BuffList = PacketList<Buff>;

	PacketTestWriter(uint64 ID, uint32 hp, uint16 ap);

	BuffList		ReserveBuffList(uint16 buff_count);
	SendBufferRef	Close();

private:
	Packet_S_TEST* packet_ = nullptr;
	SendBufferRef	send_buffer_;
	BufferWriter	buffer_writer_;
};