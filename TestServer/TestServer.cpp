#include "pch.h"
#include "ThreadManager.h"

#include "Service.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"

#include "Job.h"
#include "Player.h"
#include "Room.h"

enum
{
	GLOBAL_QUEUE_END = 64,
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		l_global_queue_end = ::GetTickCount64() + GLOBAL_QUEUE_END;

		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DoGlobalQueue();
	}
}

int main()
{
	g_room = make_shared<Room>();
	ClientPacketHandler::Init();

	g_game_session_manager = new GameSessionManager();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		g_thread_manager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}
	DoWorkerJob(service);

	g_thread_manager->Join();
}
