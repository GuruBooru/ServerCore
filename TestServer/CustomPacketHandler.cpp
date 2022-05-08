#include "pch.h"
#include "CustomPacketHandler.h"
#include "BufferWriter.h"
#include "BufferReader.h"

void CustomPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader buf_reader(buffer, len);

	PacketHeader header;
	buf_reader >> header;

	// 함수 포인터를 활용하여 동작하는 형식으로 바꿀 것
	switch (header.id)
	{
	default:
		break;
	}
}

PacketTestWriter::PacketTestWriter(uint64 ID, uint32 hp, uint16 ap)
{
	send_buffer_ = g_send_buf_manager->Open(4096);
	buffer_writer_ = BufferWriter(send_buffer_->Buffer(), send_buffer_->AllocSize());

	packet_ = buffer_writer_.Reserve<Packet_S_TEST>();
	packet_->header.size = 0;
	packet_->header.id = S_TEST;
	packet_->id = ID;
	packet_->hp = hp;
	packet_->ap = ap;
	packet_->buff_offset = 0;
	packet_->buff_count = 0;
}

PacketTestWriter::BuffList PacketTestWriter::ReserveBuffList(uint16 buff_count)
{
	Buff* buff_list_begin = buffer_writer_.Reserve<Buff>(buff_count);
	packet_->buff_count = buff_count;
	packet_->buff_offset = (uint64)buff_list_begin - (uint64)packet_;

	return BuffList(buff_list_begin, buff_count);
}

SendBufferRef PacketTestWriter::Close()
{
	packet_->header.size = buffer_writer_.WriteSize();
	send_buffer_->Close(buffer_writer_.WriteSize());

	return send_buffer_;
}
