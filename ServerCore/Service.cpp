#include "pch.h"
#include "Service.h"
#include "Session.h"

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 max_session_count)
	: type_(type), address_(address), iocp_core_(core), session_factory_(factory), max_session_count_(max_session_count)
{
}

Service::~Service()
{
}

void Service::CloseService()
{
}

void Service::Broadcast(SendBufferRef send_buffer)
{
	WRITE_LOCK;
	for (const auto& session : sessions_)
	{
		session->Send(send_buffer);
	}
}

SessionRef Service::CreateSession()
{
	SessionRef session = session_factory_();
	session->SetService(shared_from_this());

	if (iocp_core_->Register(session) == false)
		return nullptr;

	return session;
}

void Service::AddSession(SessionRef session)
{
	WRITE_LOCK;
	session_count_++;
	sessions_.insert(session);
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	ASSERT_CRASH(sessions_.erase(session) != 0);

	session_count_--;
}


/*-------------------
		Client
-------------------*/
ClientService::ClientService(NetAddress target_addr, IocpCoreRef core, SessionFactory factory, int32 max_session_count)
	: Service(ServiceType::Client, target_addr, core, factory, max_session_count)
{
}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;

	const int32 session_count = GetMaxSessionCount();
	for (int32 i = 0; i < session_count; ++i)
	{
		SessionRef session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}


/*-------------------
		Server
-------------------*/
ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 max_session_count)
	: Service(ServiceType::Server, address, core, factory, max_session_count)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	listener_ = MakeShared<Listener>();
	if (listener_ == nullptr)
		return false;

	ServerServiceRef service = static_pointer_cast<ServerService>(shared_from_this());
	if (listener_->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	Service::CloseService();
}