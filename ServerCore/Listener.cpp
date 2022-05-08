#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

Listener::~Listener()
{
	SocketUtils::Close(socket_);

	for (AcceptEvent* accept_event : accept_events_)
	{
		Custom_Delete(accept_event);
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	service_ = service;
	if (service_ == nullptr)
		return false;

	socket_ = SocketUtils::CreateSocket();
	if (socket_ == INVALID_SOCKET)
		return false;

	if (service_->GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	if (SocketUtils::SetReuseAddress(socket_, true) == false)
		return false;

	if (SocketUtils::SetLinger(socket_, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(socket_, service_->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(socket_) == false)
		return false;

	const int32 accept_count = service_->GetMaxSessionCount();
	for (int32 i = 0; i < accept_count; ++i)
	{
		AcceptEvent* accept_event = Custom_New<AcceptEvent>();
		accept_event->owner = shared_from_this();
		accept_events_.push_back(accept_event);
		RegisterAccept(accept_event);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(socket_);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(socket_);
}

void Listener::Dispatch(IocpEvent* iocp_event, int32 num_of_bytes)
{
	ASSERT_CRASH(iocp_event->type_ == EventType::Accept);
	AcceptEvent* accept_event = static_cast<AcceptEvent*>(iocp_event);
	ProcessAccept(accept_event);
}

void Listener::RegisterAccept(AcceptEvent* accept_event)
{
	SessionRef session = service_->CreateSession();

	accept_event->Init();
	accept_event->session_ = session;

	DWORD received_bytes = 0;
	if (false == SocketUtils::AcceptEx_(socket_, session->GetSocket(), session->recv_buffer_.WritePos(), 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		OUT & received_bytes, static_cast<LPOVERLAPPED>(accept_event)))
	{
		const int32 error_code = WSAGetLastError();
		if (error_code != WSA_IO_PENDING)
		{
			RegisterAccept(accept_event);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* accept_event)
{
	SessionRef session = accept_event->session_;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), socket_))
	{
		RegisterAccept(accept_event);
		return;
	}

	SOCKADDR_IN sock_addr;
	int32 sock_addr_size = sizeof(sock_addr);
	if (SOCKET_ERROR == getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sock_addr), &sock_addr_size))
	{
		RegisterAccept(accept_event);
		return;
	}

	session->SetNetAddress(NetAddress(sock_addr));
	session->ProcessConnect();

	cout << "Client Connected" << endl;

	RegisterAccept(accept_event);
}