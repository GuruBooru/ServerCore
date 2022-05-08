#pragma once
class Player
{
public:
	uint64					player_id_ = 0;
	string					name_;
	Protocol::PlayerType	type_ = Protocol::PLAYER_TYPE_NONE;
	ClientSessionRef		owner_session_;
};

