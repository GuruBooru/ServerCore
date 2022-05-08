#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "Room.h"
#include "GameSession.h"

/*-------------------------
		Protobuf
-------------------------*/
PacketHandlerFunc g_packet_handler[UINT16_MAX];

bool ClientPacketHandler::HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
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

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& packet)
{
	ClientSessionRef client_session = static_pointer_cast<GameSession>(session);

	// 유효성 검사

	Protocol::S_LOGIN login_packet;
	login_packet.set_success(true);

	static Atomic<uint64> id_generator = 1;

	{
		auto player = login_packet.add_players();
		player->set_name(u8"test_name");
		player->set_player_type(Protocol::PLAYER_TYPE_KNIGHT);

		PlayerRef player_ref = MakeShared<Player>();
		player_ref->player_id_ = id_generator++;
		player_ref->name_ = player->name();
		player_ref->type_ = player->player_type();
		player_ref->owner_session_ = client_session;

		client_session->players_.push_back(player_ref);
	}

	auto send_buffer = ClientPacketHandler::MakeSendBuffer(login_packet);
	session->Send(send_buffer);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& packet)
{
	ClientSessionRef client_session = static_pointer_cast<GameSession>(session);

	uint64 index = packet.player_index();

	PlayerRef player = client_session->players_[index];
	
	g_room->DoAsync(&Room::Enter, player);

	Protocol::S_ENTER_GAME enter_game_packet;
	enter_game_packet.set_success(true);
	auto send_buffer = ClientPacketHandler::MakeSendBuffer(enter_game_packet);
	player->owner_session_->Send(send_buffer);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& packet)
{
	cout << packet.msg() << endl;

	Protocol::S_CHAT chat_packet;
	chat_packet.set_msg(packet.msg());
	auto send_buffer = ClientPacketHandler::MakeSendBuffer(chat_packet);

	g_room->DoAsync(&Room::Broadcast, send_buffer);

	return true;
}