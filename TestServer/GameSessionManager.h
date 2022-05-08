#pragma once

class GameSession;
using GameSessionRef = shared_ptr<GameSession>;

class GameSessionManager
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef send_buffer);

private:
	USE_LOCK;
	Set<GameSessionRef> sessions_;
};

extern GameSessionManager* g_game_session_manager;
