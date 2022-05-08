#include "pch.h"
#include "CustomPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

void CustomPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	if (len < sizeof(PacketHeader))
		return;

	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// 함수 포인터를 활용하여 동작하는 형식으로 바꿀 것
	switch (header->id)
	{
	case S_TEST:
		ProcessTest(buffer, len);
		break;
	}
}

#pragma pack(1)
struct Packet_S_TEST
{
	struct Buff
	{
		uint64 id;
		uint64 remain_time;
	};

	using BuffList = PacketList<Packet_S_TEST::Buff>;

	PacketHeader	header;
	uint64			id;
	uint32			hp;
	uint16			ap;

	uint16			buff_offset;
	uint16			buff_count;

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(Packet_S_TEST);
		size += buff_count * sizeof(Buff);
		if (header.size < size)
			return false;

		if (size != header.size)
			return false;

		if (buff_offset + buff_count * sizeof(Buff) > header.size)
			return false;

		return true;
	}

	BuffList GetBuffList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buff_offset;

		return BuffList(reinterpret_cast<Buff*>(data), buff_count);
	}
};
#pragma pack()

void CustomPacketHandler::ProcessTest(BYTE* buffer, int32 len)
{

	Packet_S_TEST* packet = reinterpret_cast<Packet_S_TEST*>(buffer);
	if (packet->Validate() == false)
		return;

	Packet_S_TEST::BuffList buffs = packet->GetBuffList();
	cout << "Buff Count : " << buffs.Size() << endl;
	for (auto buff : buffs)
	{
		cout << " BufInfo : " << buff.id << " " << buff.remain_time << endl;
	}
}
