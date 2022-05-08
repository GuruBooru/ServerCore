#pragma once

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sock_addr);
	NetAddress(wstring ip, uint16 port);

	SOCKADDR_IN& GetSockAddr() { return sock_addr_; }
	wstring			GetIpAddress();
	uint16			GetPort() { return ntohs(sock_addr_.sin_port); }


	static IN_ADDR	Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN		sock_addr_ = {};
};