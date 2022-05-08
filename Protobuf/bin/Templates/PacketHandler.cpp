#include "pch.h"
#include "{{output}}.h"

/*-------------------------
		Protobuf
-------------------------*/
PacketHandlerFunc g_packet_handler[UINT16_MAX];

void {{output}}::Init()
{
	for (int32 i = 0; i < UINT16_MAX; ++i)
	{
		g_packet_handler[i] = Handle_INVALID;
	}

{%- for packet in parser.recv_packet %}
	g_packet_handler[PACKET_{{packet.name}}] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {
		return HandlePacket<Protocol::{{packet.name}}> (Handle_{{packet.name}}, session, buffer, len);
	};
{%- endfor %}
}

bool {{output}}::HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	return g_packet_handler[header->id](session, buffer, len);
}

{%- for packet in parser.send_packet %}
SendBufferRef {{output}}::MakeSendBuffer(Protocol::{{packet.name}}& packet)
{
	return MakeSendBuffer(packet, PACKET_{{packet.name}});
}
{%- endfor %}

/*--------------------------
		Packet Handle
--------------------------*/

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}

{%- for packet in parser.recv_packet %}

bool Handle_{{packet.name}}(PacketSessionRef& session, Protocol::{{packet.name}}& packet)
{
	return true;
}
{%- endfor %}
