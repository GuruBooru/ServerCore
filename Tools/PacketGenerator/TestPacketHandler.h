#pragma once
#include "BufferWriter.h"
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[UINT16_MAX];

enum : uint16
{
	PACKET_S_TEST = 1000,
	PACKET_S_LOGIN = 1001,
};

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

class TestPacketHandler
{
public:
	/*-------------------------
			Protobuf
	-------------------------*/
	static void	Init()
	{
		for (int32 i = 0; i < UINT16_MAX; ++i)
		{
			g_packet_handler[i] = Handle_INVALID;
		}
	}
	static bool				HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len);
	static SendBufferRef	MakeSendBuffer(Protocol::S_TEST&packet) { return MakeSendBuffer(packet, PACKET_S_TEST); }
	static SendBufferRef	MakeSendBuffer(Protocol::S_LOGIN&packet) { return MakeSendBuffer(packet, PACKET_S_LOGIN); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType packet;
		if (packet.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHandler)) == false)
			return false;

		return func(session, packet);
	}

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