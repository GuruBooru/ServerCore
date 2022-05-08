#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager* g_game_session_manager;

void GameSessionManager::Add(GameSessionRef session)
{
	WRITE_LOCK;
	sessions_.insert(session);
}

void GameSessionManager::Remove(GameSessionRef session)
{
	WRITE_LOCK;
	sessions_.erase(session);
}

void GameSessionManager::Broadcast(SendBufferRef send_buffer)
{
	WRITE_LOCK;
	for (GameSessionRef session : sessions_)
	{
		session->Send(send_buffer);
	}
}
