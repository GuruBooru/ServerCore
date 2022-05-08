#include "pch.h"
#include "PacketHandler.h"

/*-------------------------
		Protobuf
-------------------------*/
PacketHandlerFunc g_packet_handler[UINT16_MAX];

void TestPacketHandler::Init()
{
	for (int32 i = 0; i < UINT16_MAX; ++i)
	{
		g_packet_handler[i] = Handle_INVALID;
	}
}

bool TestPacketHandler::HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	return g_packet_handler[header->id](session, buffer, len);
}
SendBufferRef TestPacketHandler::MakeSendBuffer(Protocol::S_TEST& packet)
{
	return MakeSendBuffer(packet, PACKET_S_TEST);
}
SendBufferRef TestPacketHandler::MakeSendBuffer(Protocol::S_LOGIN& packet)
{
	return MakeSendBuffer(packet, PACKET_S_LOGIN);
}

/*--------------------------
		Packet Handle
--------------------------*/

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}