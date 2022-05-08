#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class ServiceType : uint8
{
	Server,
	Client
};

using SessionFactory = function<SessionRef(void)>;

class Service : public enable_shared_from_this<Service>
{
public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 max_session_count = 1);
	virtual ~Service();

	virtual bool	Start() abstract;
	bool			CanStart() { return session_factory_ != nullptr; }

	virtual void	CloseService();
	void			SetSessionFactory(SessionFactory factory) { session_factory_ = factory; }

	void			Broadcast(SendBufferRef send_buffer);
	SessionRef		CreateSession();
	void			AddSession(SessionRef session);
	void			ReleaseSession(SessionRef session);
	int32			GetCurrentSessionCount() { return session_count_; }
	int32			GetMaxSessionCount() { return max_session_count_; }

	ServiceType		GetServiceType() { return type_; }
	NetAddress		GetNetAddress() { return address_; }
	IocpCoreRef&	GetIocpCore() { return iocp_core_; }


protected:
	USE_LOCK;
	ServiceType			type_;
	NetAddress			address_ = {};
	IocpCoreRef			iocp_core_;

	Set<SessionRef>		sessions_;
	int32				session_count_ = 0;
	int32				max_session_count_ = 0;
	SessionFactory		session_factory_ = nullptr;
};


/*-------------------
		Client
-------------------*/
class ClientService : public Service
{
public:
	ClientService(NetAddress target_addr, IocpCoreRef core, SessionFactory factory, int32 max_session_count = 1);
	virtual ~ClientService() {}

	virtual bool Start() override;
};


/*-------------------
		Server
-------------------*/
class ServerService : public Service
{
public:
	ServerService(NetAddress target_addr, IocpCoreRef core, SessionFactory factory, int32 max_session_count = 1);
	virtual ~ServerService() {}

	virtual bool Start() override;
	virtual void CloseService() override;

private:
	ListenerRef	listener_ = nullptr;
};