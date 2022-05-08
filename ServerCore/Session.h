#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000,
	};

public:
	Session();
	virtual ~Session();

	void					Send(SendBufferRef send_buffer);
	bool					Connect();
	void					Disconnect(const WCHAR* cause);

	shared_ptr<Service>		GetService() { return service_.lock(); }
	void					SetService(shared_ptr<Service> service) { service_ = service; }

	void					SetNetAddress(NetAddress address) { net_addr_ = address; }
	NetAddress				GetNetAddress() { return net_addr_; }
	SOCKET					GetSocket() { return socket_; }
	bool					IsConnected() { return connected_; }
	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

protected:
	virtual void	OnConnected() {}
	virtual void	OnDisconnected() {}
	virtual int32	OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void	OnSend(int32 len) {}

private:
	weak_ptr<Service>		service_;
	SOCKET					socket_ = INVALID_SOCKET;
	NetAddress				net_addr_ = {};
	Atomic<bool>			connected_ = false;

	USE_LOCK;
	RecvBuffer				recv_buffer_;

	Queue<SendBufferRef>	send_queue_;
	Atomic<bool>			send_registered_ = false;

	ConnectEvent			connect_event_;
	DisconnectEvent			disconnect_event_;
	RecvEvent				recv_event_;
	SendEvent				send_event_;

private:
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* iocp_event, int32 num_of_bytes = 0) override;

	bool				RegisterConnect();
	bool				RegisterDisconnect();
	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessDisconnect();
	void				ProcessRecv(int32 num_of_bytes);
	void				ProcessSend(int32 num_of_bytes);

	void				CheckError(int32 error_code);
};


struct PacketHeader
{
	uint16 size;
	uint16 id;
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32	OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void	OnRecvPacket(BYTE* buffer, int32 len) abstract;
};