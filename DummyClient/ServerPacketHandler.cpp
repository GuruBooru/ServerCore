#include "pch.h"
#include "ServerPacketHandler.h"

/*-------------------------
		Protobuf
-------------------------*/
PacketHandlerFunc g_packet_handler[UINT16_MAX];

bool ServerPacketHandler::HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	return g_packet_handler[header->id](session, buffer, len);
}

/*--------------------------
		Packet Handle
--------------------------*/

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& packet)
{
	if (packet.success() == false)
		return false;

	if(packet.players().size() == 0)
	{
		
	}

	Protocol::C_ENTER_GAME enter_game_packet;
	enter_game_packet.set_player_index(0);
	auto send_buffer = ServerPacketHandler::MakeSendBuffer(enter_game_packet);
	session->Send(send_buffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& packet)
{
	if (packet.success() == false)
		return false;



	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& packet)
{
	cout << packet.msg() << endl;
	return true;
}