#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX		SocketUtils::ConnectEx_ = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx_ = nullptr;
LPFN_ACCEPTEX		SocketUtils::AcceptEx_ = nullptr;

void SocketUtils::Init()
{
	WSAData wsa_data;
	ASSERT_CRASH(WSAStartup(MAKEWORD(2, 2), OUT & wsa_data) == 0);

	SOCKET dummy_socket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(dummy_socket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx_)));
	ASSERT_CRASH(BindWindowsFunction(dummy_socket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx_)));
	ASSERT_CRASH(BindWindowsFunction(dummy_socket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx_)));

	Close(dummy_socket);
}

void SocketUtils::Clear()
{
	WSACleanup();
}

SOCKET SocketUtils::CreateSocket()
{
	return WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	return SOCKET_ERROR != WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
}

bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listen_socket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listen_socket);
}

bool SocketUtils::Bind(SOCKET socket, NetAddress net_addr)
{
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&net_addr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&address), sizeof(address));
}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != listen(socket, backlog);
}

void SocketUtils::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
		closesocket(socket);

	socket = INVALID_SOCKET;
}
