#pragma once
#include "BufferWriter.h"
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[UINT16_MAX];

enum : uint16
{
	PACKET_C_LOGIN = 1000,
	PACKET_S_LOGIN = 1001,
	PACKET_C_ENTER_GAME = 1002,
	PACKET_S_ENTER_GAME = 1003,
	PACKET_C_CHAT = 1004,
	PACKET_S_CHAT = 1005,
};

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& packet);
bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& packet);
bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& packet);

class ServerPacketHandler
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
		g_packet_handler[PACKET_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {
			return HandlePacket < Protocol::S_LOGIN > (Handle_S_LOGIN, session, buffer, len);
		};
		g_packet_handler[PACKET_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {
			return HandlePacket < Protocol::S_ENTER_GAME > (Handle_S_ENTER_GAME, session, buffer, len);
		};
		g_packet_handler[PACKET_S_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {
			return HandlePacket < Protocol::S_CHAT > (Handle_S_CHAT, session, buffer, len);
		};
	}
	static bool				HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len);
	static SendBufferRef	MakeSendBuffer(Protocol::C_LOGIN&packet) { return MakeSendBuffer(packet, PACKET_C_LOGIN); }
	static SendBufferRef	MakeSendBuffer(Protocol::C_ENTER_GAME&packet) { return MakeSendBuffer(packet, PACKET_C_ENTER_GAME); }
	static SendBufferRef	MakeSendBuffer(Protocol::C_CHAT&packet) { return MakeSendBuffer(packet, PACKET_C_CHAT); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType packet;
		if (packet.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
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