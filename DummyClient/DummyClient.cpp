#include "pch.h"
#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "ServerPacketHandler.h"

char dummy_data[] = "Hello World!";

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override 
	{
		Protocol::C_LOGIN packet;
		auto send_buffer = ServerPacketHandler::MakeSendBuffer(packet);
		Send(send_buffer);
	}

	virtual void OnDisconnected() override {}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		ServerPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override {}
};

int main()
{
	ServerPacketHandler::Init();
	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		g_thread_manager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}


	Protocol::C_CHAT chat_packet;
	chat_packet.set_msg(u8"Hello World!");
	auto send_buffer = ServerPacketHandler::MakeSendBuffer(chat_packet);

	while (true)
	{
		service->Broadcast(send_buffer);
		this_thread::sleep_for(1s);
	}

	g_thread_manager->Join();
}