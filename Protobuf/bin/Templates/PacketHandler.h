#pragma once
#include "BufferWriter.h"
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[UINT16_MAX];

enum : uint16
{
{%- for packet in parser.total_packet %}
	PACKET_{{packet.name}} = {{packet.id}},
{%- endfor %}
};

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

{%- for packet in parser.recv_packet %}
bool Handle_{{packet.name}}(PacketSessionRef& session, Protocol::{{packet.name}}& packet);
{%- endfor %}

class {{output}}
{
public:
	/*-------------------------
			Protobuf
	-------------------------*/
	static void	Init()
	{
		for (int32 i = 0; i < UINT16_MAX; ++i)
		{
			g_packet_handler[i] = Handle_INVALID;
		}

{%- for packet in parser.recv_packet %}
		g_packet_handler[PACKET_{{packet.name}}] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {
			return HandlePacket < Protocol::{{packet.name}} > (Handle_{{packet.name}}, session, buffer, len);
		};
{%- endfor %}
	}
	static bool				HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len);

{%- for packet in parser.send_packet %}
	static SendBufferRef	MakeSendBuffer(Protocol::{{packet.name}}&packet) { return MakeSendBuffer(packet, PACKET_{{packet.name}}); }
{%- endfor %}

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType packet;
		if (packet.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, packet);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& packet, uint16 packet_id)
	{
		const uint16 data_size = static_cast<uint16>(packet.ByteSizeLong());
		const uint16 packet_size = data_size + sizeof(PacketHeader);

		SendBufferRef send_buffer = g_send_buf_manager->Open(packet_size);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(send_buffer->Buffer());
		header->size = packet_size;
		header->id = packet_id;

		ASSERT_CRASH(packet.SerializeToArray(&header[1], data_size));

		send_buffer->Close(packet_size);
		return send_buffer;
	}
};
