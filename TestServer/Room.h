#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef send_buffer);

private:
	USE_LOCK;
	map<uint64, PlayerRef> players_;

	// JobQueue jobs_;
};

extern shared_ptr<Room> g_room;
