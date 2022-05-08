#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(SOCKADDR_IN sock_addr) : sock_addr_(sock_addr)
{
}

NetAddress::NetAddress(wstring ip, uint16 port)
{
	memset(&sock_addr_, 0, sizeof(sock_addr_));
	sock_addr_.sin_family = AF_INET;
	sock_addr_.sin_addr = Ip2Address(ip.c_str());
	sock_addr_.sin_port = htons(port);
}

wstring NetAddress::GetIpAddress()
{
	WCHAR buffer[100];
	InetNtopW(AF_INET, &sock_addr_, buffer, len32(buffer));

	return wstring(buffer);
}

IN_ADDR NetAddress::Ip2Address(const WCHAR* ip)
{
	IN_ADDR address;
	InetPtonW(AF_INET, ip, &address);
	return address;
}
