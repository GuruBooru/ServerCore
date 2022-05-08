#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

shared_ptr<Room> g_room = nullptr;

void Room::Enter(PlayerRef player)
{
	players_[player->player_id_] = player;
}

void Room::Leave(PlayerRef player)
{
	players_.erase(player->player_id_);
}

void Room::Broadcast(SendBufferRef send_buffer)
{
	for (auto& p : players_)
	{
		p.second->owner_session_->Send(send_buffer);
	}
}

