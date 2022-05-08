#pragma once
#include "NetAddress.h"

class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx_;
	static LPFN_DISCONNECTEX	DisconnectEx_;
	static LPFN_ACCEPTEX		AcceptEx_;
public:
	static void		Init();
	static void		Clear();

	static SOCKET	CreateSocket();
	static bool		BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);

	static bool		SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool		SetReuseAddress(SOCKET socket, bool flag);
	static bool		SetRecvBufferSize(SOCKET socket, int32 size);
	static bool		SetSendBufferSize(SOCKET socket, int32 size);
	static bool		SetTcpNoDelay(SOCKET socket, bool flag);
	static bool		SetUpdateAcceptSocket(SOCKET socket, SOCKET listen_socket);

	static bool		Bind(SOCKET socket, NetAddress net_addr);
	static bool		BindAnyAddress(SOCKET socket, uint16 port);
	static bool		Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void		Close(SOCKET& socket);
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 opt_name, T opt_val)
{
	return SOCKET_ERROR != setsockopt(socket, level, opt_name, reinterpret_cast<char*>(&opt_val), sizeof(T));
}
